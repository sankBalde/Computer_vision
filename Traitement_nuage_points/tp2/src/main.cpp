#include <kdtree.h>
#include <obj.h>
#include <geometry.h>
#include <pivot.h>
#include <init.h>
#include <set>
#include <stack>

// Edge e_ij oriented from point i to point j
// point opp is the 3rd vertex of the triangle defined by this edge
// ball_center is the center of the ball associated to this edge
// next and prev allow to consider a doubly linked list
// to_be_removed is set to true if the edge is no longer needed
struct Edge
{
    int from, to, opp;
    Eigen::Vector3f ball_center;
    Edge* next{nullptr};
    Edge* prev{nullptr};
    bool to_be_removed{false};
};

enum class Status
{
    FREE,
    FRONT,
    INSIDE
};

using namespace tnp;

int main(int argc, char *argv[])
{
    // Option -----------------------------------------------------------------
    if(argc <= 1) {
        std::cout << "Error: missing argument" << std::endl;
        std::cout << "Usage: ball_pivoting <filename>.obj" << std::endl;
        return 0;
    }
    const std::string filename = argv[1];
    const float ball_radius = 0.25f;

    // Load -------------------------------------------------------------------
    std::vector<Eigen::Vector3f> points;
    if (!tnp::load_obj(filename, points)) {
        std::cout << "Failed to open input file '" << filename << "'" << std::endl;
        return 1;
    }

    // Process ----------------------------------------------------------------
    std::vector<std::vector<Edge*>> outter_edges(points.size());
    std::vector<Eigen::Vector3i> faces;

    tnp::KdTree kdtree;
    kdtree.build(points);

    // Initialize structures
    std::vector<Status> statuses(points.size(), Status::FREE);

    std::stack<Edge*> stack;



    // Get the initial triangle
    auto [init_triangle, init_center] = initial_triangle(points, kdtree, ball_radius);
    const int i = init_triangle[0];
    const int j = init_triangle[1];
    const int k = init_triangle[2];

    // Create initial edges and set statuses
    Edge* e1 = new Edge{i, j, k, init_center};
    Edge* e2 = new Edge{j, k, i, init_center};
    Edge* e3 = new Edge{k, i, j, init_center};

    e1->next = e2; e2->prev = e1;

    e2->next = e3; e3->prev = e2;
    e3->next = e1; e1->prev = e3;

    stack.push(e1);
    stack.push(e2);
    stack.push(e3);

    statuses[i] = Status::FRONT;
    statuses[j] = Status::FRONT;
    statuses[k] = Status::FRONT;

    faces.emplace_back(i, j, k);
    // Propagate the front
    int iteration_count = 0;
    const int max_iterations = 5000000; // Limit iterations to prevent infinite loops

    while (!stack.empty() && iteration_count < max_iterations) {
        ++iteration_count;
        Edge* ij = stack.top();
        stack.pop();
        // Etape 6
        if (ij->to_be_removed) {
            delete ij;
            continue;
        }
        /*if (statuses[ij->from] != Status::FRONT || statuses[ij->to] != Status::FRONT) {
            continue;
        }*/
        // Etape 10 : can not pivot
        auto result = pivot(ij->from, ij->to, ij->opp, ij->ball_center, points, kdtree, ball_radius);
        if (!result) {
            continue;
        }


        int new_point = result->first;
        Eigen::Vector3f new_center = result->second;
        if (statuses[new_point] == Status::FRONT) {
            continue;
        }

        Edge* lj = new Edge{new_point, ij->to, ij->from, new_center}; // lj
        Edge* il = new Edge{ij->from, new_point, ij->to, new_center}; // li

        lj->next = ij->next;
        lj->prev = il;
        il->prev = ij->prev;
        il->next = lj;
        ij->prev->next = il;
        ij->next->prev = lj;

        stack.push(il);
        stack.push(lj);

        statuses[new_point] = Status::FRONT;
        statuses[ij->to] = Status::FRONT;
        ij->to_be_removed = true;
        faces.emplace_back(ij->from, ij->to, new_point);
        //delete ij;
    }

    if (iteration_count >= max_iterations) {
        std::cerr << "Error: Maximum iterations reached. Exiting early to avoid infinite loop." << std::endl;
    }

    // Save the mesh
    tnp::save_obj("mesh.obj", points, faces);

    return 0;
}
