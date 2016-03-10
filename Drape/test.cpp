#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>
#include <cmath>
#include <time.h>
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point_d;
typedef CGAL::Search_traits_3<K> TreeTraits;
typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
typedef Neighbor_search::Tree Tree;
#define Random(from, to) (from + (to-from)/rand()%to)
int _main() {
	const unsigned int N = 1;
	std::list<Point_d> points;
	for(int i = 0; i < 200; i++)
	{
		double x = rand()%1000;
		double y = rand()%1000;
		double z = rand()%1000;
		points.push_back(Point_d(x,y,z));
	}
	
	Tree tree(points.begin(), points.end());
	Point_d query(121,23,41);
	// Initialize the search structure, and search all N points

	Neighbor_search search(tree, query, N);
	for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		std::cout << "Point: " << it->first << "\n";
		std::cout << "Distance: " << std::sqrt(it->second) << "\n";
	}

	getchar();
	int queryTime;
	std::vector<Point_d> container; 
	while(std::cin >> queryTime)
	{
		container.clear();
		clock_t start, end;
		start = clock();
		for(int i = 0; i < queryTime; i++)
		{
			Neighbor_search search(tree, query, N);
		 	for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
			{
		 		//std::cout << it->first << " "<< std::sqrt(it->second) << std::endl;
				container.push_back(it->first);
		 	}
		}
		end = clock();
		// report the N nearest neighbors and their distance
		// This should sort all N points by increasing distance from origin
		// 	for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it){
		// 		std::cout << it->first << " "<< std::sqrt(it->second) << std::endl;
		// 	}
		std::cout << (end - start)/1000 << 's' << std::endl;
		
	}

	getchar();
	return 0;
}