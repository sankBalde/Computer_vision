#include <kdtree.h>
#include <stack>

namespace tnp {

KdTree::~KdTree()
{
    this->delete_rec(m_root);
}

// build the kdtree (without modifying the points)
void KdTree::build(const std::vector<Eigen::Vector3f>& points)
{
    // initialize indices with 0, 1, 2, ..., n-1
    m_indices.resize(points.size());
    std::iota(m_indices.begin(), m_indices.end(), 0);

    // allocate root node and recursively build the tree
    m_root = new Node();
    this->build_rec(points, m_indices.begin(), m_indices.end(), m_root);
}


// recursively build the tree
// consider points at indices in the range (begin,end(
// node is already allocated and must be filled
// points are not changed
void KdTree::build_rec(
    const std::vector<Eigen::Vector3f>& points, // point cloud
    iterator begin,                             // begin iterator over the current points indices
    iterator end,                               // end iterator over the current points indices
    Node* node)                                 // node to fill 
{
    // assert(node != nullptr);
    // assert(begin <= end);

    // step 1.1

    // Condition d'arrêt : si le nombre de points est inférieur ou égal à max_number_point_per_leaf
    if (std::distance(begin, end) <= max_number_point_per_leaf) {
        node->begin = begin;
        node->end = end;
        node->left_child = nullptr;
        node->right_child = nullptr;
        return;
    }

    // Calculer la boîte englobante des points dans l'intervalle [begin, end[
    Box3f bounding_box;
    for (auto it = begin; it != end; ++it) {
        bounding_box.extend(points[*it]);
    }

    // Trouver la dimension avec la plus grande étendue
    Eigen::Vector3f diagonal = bounding_box.diagonal();
    node->cut_dim = 0;
    if (diagonal[1] > diagonal[0]) node->cut_dim = 1;
    if (diagonal[2] > diagonal[node->cut_dim]) node->cut_dim = 2;

    // Calculer la valeur de coupe (au milieu de la boîte englobante dans la dimension choisie)
    node->cut_value = (bounding_box.min()[node->cut_dim] + bounding_box.max()[node->cut_dim]) / 2.0f;

    // Partitionner les indices en fonction de la valeur de coupe
    auto sep = std::partition(begin, end, [&](int idx) {
        return points[idx][node->cut_dim] < node->cut_value;
    });

    // Vérification des prédicats
    assert(std::all_of(begin, sep, [&](int idx) {
        return points[idx][node->cut_dim] < node->cut_value;
    }));
    assert(std::all_of(sep, end, [&](int idx) {
        return points[idx][node->cut_dim] >= node->cut_value;
    }));

    // Allouer les nœuds enfants
    node->left_child = new Node();
    node->right_child = new Node();

    // Construire récursivement les sous-arbres
    build_rec(points, begin, sep, node->left_child);
    build_rec(points, sep, end, node->right_child);
}



//
// neighbors range search from point p and distance r
// call f on each point at index i such that (p - points[i]).norm() < r
//
// Example: 
//     kdtree.for_each_neighbors(points, p, r, [&points](int i)
//     {
//         std::cout << "Found point " << i << ": " << points[i].transpose() << std::endl;
//     });
//
void KdTree::for_each_neighbors(
    const std::vector<Eigen::Vector3f>& points, // point cloud
    const Eigen::Vector3f& p,                   // query point
    float r,                                    // query radius
    Func f) const                               // function to called on resulting indices
{
    // stack used for iterative depth traversal
    //std::stack<Node*> stack;
    //stack.push(m_root);

    // step 1.2
    // Pile pour traverser l'arbre de manière itérative
    std::stack<std::pair<Node*, Box3f>> stack;

    // Initialisation : commencer par le nœud racine et une boîte englobante infinie
    Box3f root_box;
    root_box.setEmpty(); // Une boîte vide qui s'étend avec les points
    stack.push({m_root, root_box});

    // Distance au carré pour éviter de recalculer les racines carrées inutilement
    float r_squared = r * r;

    // Parcours en profondeur
    while (!stack.empty()) {
        auto [node, box] = stack.top();
        stack.pop();

        // Si le nœud est une feuille
        if (!node->left_child && !node->right_child) {
            for (auto it = node->begin; it != node->end; ++it) {
                int index = *it;
                if ((points[index] - p).squaredNorm() < r_squared) {
                    f(index); // Appeler la fonction sur l'index si le point est dans la sphère
                }
            }
            continue;
        }

        // Récupérer la valeur de coupe et la dimension
        float cut_value = node->cut_value;
        int cut_dim = node->cut_dim;
        float ref = p[cut_dim];

        // Vérifier les conditions pour explorer les branches gauche et droite
        if (ref - r <= cut_value) {
            stack.push({node->left_child, box});
        }
        if (ref + r >= cut_value) {
            stack.push({node->right_child, box});
        }
    }
}

// recursively delete nodes
void KdTree::delete_rec(Node* node)
{
    // step 1.3
    if (!node) {
        return; // Si le nœud est nul, rien à supprimer
    }

    // Supprimer récursivement les enfants gauche et droit
    delete_rec(node->left_child);
    delete_rec(node->right_child);

    // Supprimer le nœud actuel
    delete node;
}

} // namespace tnp3