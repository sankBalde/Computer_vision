#include <kdtree.h>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

int main()
{
    // 0. initialization of a random 3D point cloud with N points uniformly distributed in (-1,+1)^3
    constexpr auto N = 1000000;
    auto points = std::vector<Eigen::Vector3f>(N);
    std::generate(points.begin(), points.end(), [](){return Eigen::Vector3f::Random();});
    
    // 1. kdtree build
    tnp::KdTree kdtree;
    kdtree.build(points);

    // 2. neighbors query from point p at radius r
    const auto p = Eigen::Vector3f::Zero().eval();
    const auto r = 0.5;

    // 3. perform query using kdtree
    const auto start0 = Clock::now();
    std::vector<int> neighbors_kdtree;
    kdtree.for_each_neighbors(points, p, r, [&](int i) {
        neighbors_kdtree.push_back(i);}
    );
    const auto stop0 = Clock::now();
    const long long int t0 = std::chrono::duration_cast<std::chrono::microseconds>(stop0 - start0).count(); 

    // 4. perform query using naive search (loop over all points)
    const auto start1 = Clock::now();
    std::vector<int> neighbors_naive;
    for(auto i = 0u; i < points.size(); ++i) 
        if((p - points[i]).norm() < r) 
            neighbors_naive.push_back(i);
    const auto stop1 = Clock::now();
    const long long int t1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1).count(); 

    std::cout << "time kdtree = " << t0 << std::endl; 
    std::cout << "time naive  = " << t1 << std::endl; 

    // 5. check by comparing (sorted) indices
    std::sort(neighbors_kdtree.begin(), neighbors_kdtree.end());
    std::sort(neighbors_naive.begin(), neighbors_naive.end());
    if(neighbors_kdtree != neighbors_naive) { 
        std::cout << "Error!" << std::endl;
        std::cout << "Naive search found " << neighbors_naive.size() << " points ( ";
        for(auto i : neighbors_naive)
            std::cout << i << " ";
        std::cout << ")" << std::endl;
        std::cout << "Kdtree search found " << neighbors_kdtree.size() << " points (";
        for(auto i : neighbors_kdtree)
            std::cout << i << " ";
        std::cout << ")" << std::endl;        
    } else {
        std::cout << "Success: found " << neighbors_kdtree.size() << " neighbors among " << N << " points" << std::endl;
    } 
    
    return 0;
}



