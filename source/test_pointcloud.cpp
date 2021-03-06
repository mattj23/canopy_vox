#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <set>

#include "pointcloud.h"
#include "vector3d.h"
#include "nanoflann.hpp"

using namespace nanoflann;

PointCloud buildCloud()
{
    PointCloud cloud;
    cloud.pts.push_back(Vector3d(0, 0, 0));
    cloud.pts.push_back(Vector3d(1.5, 0, 0));
    cloud.pts.push_back(Vector3d(0, 0, 1.5));
    cloud.pts.push_back(Vector3d(-10, 0, 0));
    cloud.pts.push_back(Vector3d(0, 10, 0));
    cloud.pts.push_back(Vector3d(0, 0, 10));
    cloud.pts.push_back(Vector3d(2, 0, 0));
    cloud.pts.push_back(Vector3d(0, 2, 0));
    cloud.pts.push_back(Vector3d(0, 0, 2));
    cloud.pts.push_back(Vector3d(0, 1.5, 0));
    return cloud;
}

PointCloud reducedCloud()
{
    PointCloud cloud;
    cloud.pts.push_back(Vector3d(0, 0, 0));
    cloud.pts.push_back(Vector3d(-10, 0, 0));
    cloud.pts.push_back(Vector3d(0, 10, 0));
    cloud.pts.push_back(Vector3d(0, 0, 10));
    cloud.pts.push_back(Vector3d(2, 0, 0));
    cloud.pts.push_back(Vector3d(0, 2, 0));
    cloud.pts.push_back(Vector3d(0, 0, 2));
    return cloud;
}

TEST (PointCloud, BuildCloud)
{
    PointCloud cloud;
    cloud.pts.push_back(Vector3d());
    ASSERT_EQ(1, cloud.pts.size());
}

TEST (PointCloud, BuildCloudMany)
{
    auto cloud = buildCloud();
    ASSERT_EQ(10, cloud.pts.size());
}

TEST (PointCloud, NanoflannBuildIndex)
{
    auto cloud = buildCloud();
    // construct a kd-tree index:
	typedef KDTreeSingleIndexAdaptor<
		L2_Simple_Adaptor<double, PointCloud> ,
		PointCloud,
		3 /* dim */
		> my_kd_tree_t;

	my_kd_tree_t   index(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
	index.buildIndex();
}

TEST (PointCloud, NanoflannRadiusSearchClose)
{

    auto cloud = buildCloud();
    // construct a kd-tree index:
	typedef KDTreeSingleIndexAdaptor<
		L2_Simple_Adaptor<double, PointCloud> ,
		PointCloud,
		3 /* dim */
		> my_kd_tree_t;

	my_kd_tree_t   index(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
	index.buildIndex();

    double query_pt[3] = { 0.0, 0.0, 0.0};
    const double radius = 1.6;

	std::vector<std::pair<size_t,double>> indices_dists;
	RadiusResultSet<double,size_t> resultSet(radius*radius, indices_dists);
	index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

    ASSERT_EQ(4, resultSet.m_indices_dists.size());
    // for (auto p : resultSet.m_indices_dists)
    // {
    //     std::cout << "index = " << p.first << ", dist = " << p.second << ", element = " << cloud.pts[p.first] << std::endl;
    // }

	// Get worst (furthest) point, without sorting:
	// std::pair<size_t,double> worst_pair = resultSet.worst_item();
	// std::cout << "Worst pair: idx=" << worst_pair.first << " dist=" << worst_pair.second << std::endl;
}

TEST (PointCloud, NanoflannRadiusSearchMed)
{
    auto cloud = buildCloud();
    // construct a kd-tree index:
	typedef KDTreeSingleIndexAdaptor<
		L2_Simple_Adaptor<double, PointCloud> ,
		PointCloud,
		3 /* dim */
		> my_kd_tree_t;

	my_kd_tree_t   index(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
	index.buildIndex();

    double query_pt[3] = { 0.0, 0.0, 0.0};
    const double radius = 2.01;

	std::vector<std::pair<size_t,double>> indices_dists;
	RadiusResultSet<double,size_t> resultSet(radius*radius, indices_dists);
	index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

    ASSERT_EQ(7, resultSet.m_indices_dists.size());
}

TEST (PointCloud, Thinning)
{
    auto cloud = buildCloud();
    auto expected = reducedCloud();

    // Build the kd-tree index
    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud>, PointCloud, 3> my_kd_tree_t;
	my_kd_tree_t index(3, cloud, KDTreeSingleIndexAdaptorParams(10));
	index.buildIndex();

    const double radius = 1.51;
    std::set<size_t> removeIndicies;

    for (size_t i = 0; i < cloud.pts.size(); i++)
    {
        if (removeIndicies.find(i) == removeIndicies.end())
        {
            double query_pt[3] = { cloud.pts[i].x, cloud.pts[i].y, cloud.pts[i].z};

            std::vector<std::pair<size_t,double>> indices_dists;
            RadiusResultSet<double,size_t> resultSet(radius*radius, indices_dists);
            index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

            for (auto r : resultSet.m_indices_dists)
            {
                if (i != r.first)
                    removeIndicies.insert(r.first);
            }
        }
    }

    cloud.RemoveAtIndicies(removeIndicies);

    ASSERT_EQ(expected.size(), cloud.size());

    for (auto v : expected.pts)
    {
        ASSERT_NE(cloud.pts.end(), std::find(cloud.pts.begin(), cloud.pts.end(), v));
    }


}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
