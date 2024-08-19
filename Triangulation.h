#pragma once
#include <cassert>
#include <list>
#include <iostream>
#include <vector>

//polygon2, partition, triangulation
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/draw_polygon_2.h>
#include <CGAL/draw_triangulation_3.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_2.h>

//Arrangements
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>

//random gen
#include <CGAL/point_generators_2.h>
#include <CGAL/random_polygon_2.h>

#include "ez.h"


class Triangulation
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Partition_traits_2<K>                         Traits;
	typedef Traits::Polygon_2                                   Polygon_2;
	typedef Traits::Point_2                                     Point_2;


	typedef CGAL::Exact_predicates_inexact_constructions_kernel   Arr_Kernel;
	typedef Arr_Kernel::FT                                      Arr_Number_type;
	typedef CGAL::Arr_segment_traits_2<Arr_Kernel>              Arr_Traits;

	typedef CGAL::Arr_extended_dcel<Arr_Traits, ElementInfo, int, int> Dcel;
	typedef CGAL::Arrangement_2<Arr_Traits, Dcel> Arrangement_2;
	typedef CGAL::Arrangement_2<Arr_Traits>                     Arrangement;


	typedef CGAL::Triangulation_vertex_base_with_info_2<ElementInfo, K>    Vb;
	typedef CGAL::Triangulation_face_base_with_info_2<ElementInfo, K>      Fbb;//era fb
	typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>            Fb;  //added
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb>                   Tds;
	typedef CGAL::No_constraint_intersection_requiring_constructions_tag   Itag; // added
	typedef CGAL::Constrained_triangulation_2<K, Tds, Itag>                ConstrainedTriangulation;

	typedef std::list<Point_2>                         Container;
	typedef CGAL::Polygon_2<K, Container>              Polygon_2;
	typedef CGAL::Random_points_in_square_2<Point_2> Point_generator;


public:
	void GenerateNew();
	void CalculateArrangementVisibilityAtPoint(ez_point point);
	bool VisAtPoint(ez_point p);
	double Triangulation::GenerateNew_Runtime();
	
	void addPointFromClick(int xP, int yP);
	void clearPoly();
	void publicPointPopulation();
	
	Polygon_2 polygon;
	
	std::vector<ez_edge> edges;
	std::vector<ez_point> vertices;
	std::vector<ez_edge> monotone_edges;
	std::vector<ez_edge> triangulation_edges;
	std::vector<ez_edge> arrangement_edges;
	std::vector<ez_point> visibility_vertices;
	std::vector<Arr_Traits::Segment_2> gsegments;
	std::vector<ez_edge> dualgraph_edges;
	std::vector<ez_point> kcolor_1;
	std::vector<ez_point> kcolor_2;
	std::vector<ez_point> kcolor_3;
	std::vector<ez_edge> test_triangulation_edges;
	std::vector<ez_point> right_click_visibility;

	Triangulation()
	{
		
		polygon.push_back(Point_2(391, 374));
		polygon.push_back(Point_2(240, 431));
		polygon.push_back(Point_2(252, 340));
		polygon.push_back(Point_2(374, 320));
		polygon.push_back(Point_2(289, 214));
		polygon.push_back(Point_2(134, 390));
		polygon.push_back(Point_2(68, 186));
		polygon.push_back(Point_2(154, 259));
		polygon.push_back(Point_2(161, 107));
		polygon.push_back(Point_2(435, 108));
		polygon.push_back(Point_2(208, 148));
		polygon.push_back(Point_2(295, 160));
		polygon.push_back(Point_2(421, 212));
		polygon.push_back(Point_2(441, 303));
		

		PopulateArrangementAndDrawInternalsFromPolygon();
		YMonotonePartitionPolygon2();
		//TriangulateYMonotonePartitions();
		CustomTriangulationImplementation();

		CGAL::insert_non_intersecting_curves(Triangulation::traversalArrangement, gsegments.data(), gsegments.data() + gsegments.size());
		InternalArrangement2Ez();
		ThreeColorTraversalArrangement();
	}

private:
	std::vector<Polygon_2> y_mono_polygons;
	Arrangement visibilityArrangement;
	Arrangement_2 traversalArrangement;
	Arrangement_2 testTriangulationArrangement;
	ConstrainedTriangulation constrainedTriangulation;
	void InternalArrangement2Ez();
	void PopulateArrangementAndDrawInternalsFromPolygon();
	void YMonotonePartitionPolygon2();
	void TriangulateFace(const Arrangement_2::Face_handle fh);
	void CustomTriangulationImplementation();
	void TriangulateYMonotonePartitions();
	void ThreeColorTraversalArrangement();
	void ThreeColorVertices(Arrangement_2::Ccb_halfedge_circulator edge);
};

