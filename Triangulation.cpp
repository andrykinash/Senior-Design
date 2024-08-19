#include "Triangulation.h"
#include <chrono>
#include <CGAL/draw_polygon_2.h>
#include <CGAL/draw_triangulation_3.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/partition_2.h>

#include <CGAL/Simple_polygon_visibility_2.h>
#include <CGAL/Triangular_expansion_visibility_2.h> 
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_naive_point_location.h>

#include <algorithm>

#include <CGAL/enum.h>
#include <CGAL/number_utils.h>
#include <CGAL/squared_distance_2.h> //for 2D functions

// for do_intersect
#include <CGAL/Boolean_set_operations_2.h>
using namespace std;
using namespace std::chrono;


// Arrangements
typedef CGAL::Exact_predicates_exact_constructions_kernel   Arr_Kernel;
typedef Arr_Kernel::FT                                      Arr_Number_type;
typedef CGAL::Arr_segment_traits_2<Arr_Kernel>              Arr_Traits;


typedef CGAL::Arr_extended_dcel<Arr_Traits, int, int, int> Dcel;

typedef CGAL::Arrangement_2<Arr_Traits, Dcel> Arrangement_2;

typedef CGAL::Arrangement_2<Arr_Traits>                     Arrangement;

typedef Arr_Traits::Point_2                                 Arr_Point;
typedef Arr_Traits::X_monotone_curve_2					    Arr_Segment;
typedef Arrangement::Vertex_handle                          Arr_Vertex_handle;
typedef Arrangement::Halfedge_handle                        Arr_Halfedge_handle;
typedef Arrangement::Face_handle                            Arr_Face_handle;
typedef Arrangement::Vertex_const_handle                    Arr_Vertex_const_handle;
typedef Arrangement::Halfedge_const_handle                  Arr_Halfedge_const_handle;


// polygon, partition
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Partition_traits_2<K>                         Traits;

typedef Traits::Point_2                                     Point_2;
typedef Traits::Polygon_2                                   Polygon_2;
typedef Polygon_2::Vertex_iterator                          VertexIterator;

// constrained triangulation
typedef CGAL::Triangulation_vertex_base_with_info_2<ElementInfo, K>    Vb;
typedef CGAL::Triangulation_face_base_with_info_2<ElementInfo, K>      Fbb;//era fb
typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>            Fb;  //added
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>                   Tds;
typedef CGAL::No_constraint_intersection_requiring_constructions_tag   Itag; // added
typedef CGAL::Constrained_triangulation_2<K, Tds, Itag>                ConstrainedTriangulation;

typedef ConstrainedTriangulation::Edge_iterator                        Constrained_Edge_iterator;
typedef ConstrainedTriangulation::Constrained_edges_iterator           Constrained_edges_iterator;
typedef ConstrainedTriangulation::Face_handle                          Constrained_Face_handle;
typedef ConstrainedTriangulation::Edge                                 Constrained_Edge;
typedef ConstrainedTriangulation::Point                                Constrained_Point;


void Triangulation::TriangulateFace(const Arrangement_2::Face_handle fh)
{

	std::vector<Arrangement_2::Vertex_handle> sorted_vertices;
	auto ci = fh->outer_ccb();
	Arrangement_2::Halfedge start = *ci;
	Arrangement_2::Halfedge top = *ci;
	Arrangement_2::Halfedge curr = *++ci;

	
	while (curr.target() != start.target())
	{
		if (curr.target()->point().y() > top.target()->point().y())
		{
			top = curr;
		}

		ci++;
		curr = *ci;
	}

	//std::cout << top.target()->point().y() << std::endl;

	Arrangement_2::Vertex_handle topVert = top.target();
	topVert->data().type = TYPE::TOP;
	sorted_vertices.push_back(topVert);

	Arrangement_2::Halfedge_handle LHS, RHS;
	LHS = &top;
	RHS = top.next();

	//std::cout << "LHS = " << LHS->target()->point().y() << std::endl;
	//std::cout << "RHS = " << RHS->source()->point().y() << std::endl;


	while (LHS->source()->point() != RHS->target()->point())
	{
		if (LHS->source()->point().y() > RHS->target()->point().y())
		{
			Arrangement_2::Vertex_handle vert = LHS->source();
			vert->data().type = TYPE::LEFT;
			/*
			if (CGAL::right_turn(LHS->prev()->source()->point(), LHS->source()->point(), LHS->target()->point()))
			{
				vert->data().reflex = TRUE;
			}
			else
			{
				vert->data().reflex = FALSE;
			}
			*/
			sorted_vertices.push_back(vert);
			LHS = LHS->prev();
		}
		else if (LHS->source()->point().y() < RHS->target()->point().y())
		{
			Arrangement_2::Vertex_handle vert = RHS->target();
			vert->data().type = TYPE::RIGHT;
			/*
			if (CGAL::left_turn(RHS->next()->target()->point(), RHS->target()->point(), RHS->source()->point()))
			{
				vert->data().reflex = TRUE;
			}
			else
			{
				vert->data().reflex = FALSE;
			}
			*/
			sorted_vertices.push_back(vert);
			RHS = RHS->next();
		}
		else if (LHS->source()->point().x() < RHS->target()->point().x())
		{
			Arrangement_2::Vertex_handle vert = LHS->source();
			vert->data().type = TYPE::LEFT;
			/*
			if (CGAL::right_turn(LHS->prev()->source()->point(), LHS->source()->point(), LHS->target()->point()))
			{
				vert->data().reflex = TRUE;
			}
			else
			{
				vert->data().reflex = FALSE;
			}
			*/
			sorted_vertices.push_back(vert);
			LHS = LHS->prev();
		}
		else
		{
			Arrangement_2::Vertex_handle vert = RHS->target();
			vert->data().type = TYPE::RIGHT;
			/*
			if (CGAL::left_turn(RHS->next()->target()->point(), RHS->target()->point(), RHS->source()->point()))
			{
				vert->data().reflex = TRUE;
			}
			else
			{
				vert->data().reflex = FALSE;
			}
			*/
			sorted_vertices.push_back(vert);
			RHS = RHS->next();
		}
	}

	Arrangement_2::Vertex_handle vert = RHS->target();
	vert->data().type = TYPE::BOTTOM;
	sorted_vertices.push_back(vert);

	std::stack<Arrangement_2::Vertex_handle> vert_stack;

	vert_stack.push(sorted_vertices[0]);
	vert_stack.push(sorted_vertices[1]);
	Arrangement_2::Vertex_handle v;
	Arrangement_2::Vertex_handle last;
	ez_edge e;

	for (int i = 2; i < sorted_vertices.size(); i++)
	{
		if (sorted_vertices[i]->data().type == BOTTOM)
		{
			break;
		}

		if (sorted_vertices[i]->data().type != vert_stack.top()->data().type)
		{
			while (!vert_stack.empty())
			{
				v = vert_stack.top();
				vert_stack.pop();
				if (!vert_stack.empty()) {
					e.source.x = sorted_vertices[i]->point().x();
					e.source.y = sorted_vertices[i]->point().y();
					e.target.x = v->point().x();
					e.target.y = v->point().y();
					Triangulation::test_triangulation_edges.push_back(e);
					Polygon_2::Segment_2 s(sorted_vertices[i]->point(), v->point());
					Triangulation::gsegments.push_back(s);
				}
			}
			vert_stack.push(sorted_vertices[i - 1]);
			vert_stack.push(sorted_vertices[i]);
		}
		else
		{
			last = vert_stack.top();
			vert_stack.pop();
			while (!vert_stack.empty())
			{
				v = vert_stack.top();
				if (sorted_vertices[i]->data().type == RIGHT && CGAL::orientation(last->point(), sorted_vertices[i]->point(), v->point()) == CGAL::LEFT_TURN ||
					sorted_vertices[i]->data().type == LEFT && CGAL::orientation(last->point(), sorted_vertices[i]->point(), v->point()) == CGAL::RIGHT_TURN)
				{
					vert_stack.pop();
					e.source.x = sorted_vertices[i]->point().x();
					e.source.y = sorted_vertices[i]->point().y();
					e.target.x = v->point().x();
					e.target.y = v->point().y();
					Triangulation::test_triangulation_edges.push_back(e);
					Polygon_2::Segment_2 s(sorted_vertices[i]->point(), v->point());
					Triangulation::gsegments.push_back(s);
					last = v;
				}
				else
				{
					break;
				}
			}
			vert_stack.push(last);
			vert_stack.push(sorted_vertices[i]);

		}

	}

	if (vert_stack.size() > 2)
	{
		vert_stack.pop();
		while (vert_stack.size() > 1)
		{
			v = vert_stack.top();
			e.source.x = sorted_vertices.back()->point().x();
			e.source.y = sorted_vertices.back()->point().y();
			e.target.x = v->point().x();
			e.target.y = v->point().y();
			Triangulation::test_triangulation_edges.push_back(e);
			Polygon_2::Segment_2 s(sorted_vertices.back()->point(), v->point());
			Triangulation::gsegments.push_back(s);
			vert_stack.pop();
		}
	}

}

void Triangulation::CustomTriangulationImplementation()
{
	for (Arrangement_2::Face_handle fh = Triangulation::testTriangulationArrangement.faces_begin(); fh != Triangulation::testTriangulationArrangement.faces_end(); ++fh)
	{
		if (!fh->is_unbounded())
		{

			//std::cout << fh->number_of_outer_ccbs() << std::endl;
			TriangulateFace(fh);
		}
	}
}

void Triangulation::TriangulateYMonotonePartitions() {
	for (const Polygon_2& poly : Triangulation::y_mono_polygons) {
		//Triangulation::constrainedTriangulation.insert(poly.begin(), poly.end());
		Triangulation::constrainedTriangulation.insert_constraint(poly.begin(), poly.end(), true);
	}

	ez_edge edge;
	for (Constrained_Edge_iterator it = Triangulation::constrainedTriangulation.finite_edges_begin(); it != Triangulation::constrainedTriangulation.finite_edges_end(); it++) {

		ConstrainedTriangulation::Vertex_handle v1 = it->first->vertex((it->second + 1) % 3);
		ConstrainedTriangulation::Vertex_handle v2 = it->first->vertex((it->second + 2) % 3);
		Polygon_2::Segment_2 s(v1->point(), v2->point());

		if (Triangulation::polygon.has_on_bounded_side(CGAL::midpoint(s)))
		{
			Triangulation::gsegments.push_back(s);

			if (!Triangulation::constrainedTriangulation.is_constrained(*it))
			{
				edge.source.x = v1->point().x();
				edge.source.y = v1->point().y();
				edge.target.x = v2->point().x();
				edge.target.y = v2->point().y();

				Triangulation::triangulation_edges.push_back(edge);
			}
		}

	}
}

bool Triangulation::VisAtPoint(ez_point p) {

	Triangulation::right_click_visibility.clear();

	Point_2 q(p.x, p.y);
	Arrangement::Face_const_handle* face;
	CGAL::Arr_naive_point_location<Arrangement> pl(Triangulation::visibilityArrangement);
	CGAL::Arr_point_location_result<Arrangement>::Type obj = pl.locate(q);
	// The query point locates in the interior of a face
	face = boost::get<Arrangement::Face_const_handle>(&obj);

	if (*face == Triangulation::visibilityArrangement.unbounded_face())
	{
		return false;
	}

	// compute regularized visibility area
	// Define visibiliy object type that computes regularized visibility area
	typedef CGAL::Simple_polygon_visibility_2<Arrangement, CGAL::Tag_true> RSPV;
	Arrangement regular_output;
	RSPV regular_visibility(Triangulation::visibilityArrangement);
	Arrangement::Face_handle fh = regular_visibility.compute_visibility(q, *face, regular_output);

	Arrangement::Ccb_halfedge_circulator curr = fh->outer_ccb();

	ez_point point;
	point.x = curr->source()->point().x();
	point.y = curr->source()->point().y();
	Triangulation::right_click_visibility.push_back(point);

	while (++curr != fh->outer_ccb())
	{
		point.x = curr->source()->point().x();
		point.y = curr->source()->point().y();

		Triangulation::right_click_visibility.push_back(point);
	}

	return true;

}

void Triangulation::CalculateArrangementVisibilityAtPoint(ez_point p)
{
	Triangulation::visibility_vertices.clear();

	Point_2 target(p.x, p.y);


	Arrangement::Halfedge_const_handle he = Triangulation::visibilityArrangement.halfedges_begin();
	while (he->target()->point() != target || he->face()->is_unbounded())
		he++;


	typedef CGAL::Simple_polygon_visibility_2<Arrangement, CGAL::Tag_true> RSPV;
	typedef CGAL::Triangular_expansion_visibility_2<Arrangement>  TEV;
	Arrangement regular_output;

	TEV tev(Triangulation::visibilityArrangement);

	Arrangement::Face_handle fh = tev.compute_visibility(target, he, regular_output);
	Arrangement::Ccb_halfedge_circulator curr = fh->outer_ccb();

	ez_point point;
	point.x = curr->source()->point().x();
	point.y = curr->source()->point().y();
	Triangulation::visibility_vertices.push_back(point);

	while (++curr != fh->outer_ccb())
	{
		point.x = curr->source()->point().x();
		point.y = curr->source()->point().y();

		Triangulation::visibility_vertices.push_back(point);
	}

}

void Triangulation::PopulateArrangementAndDrawInternalsFromPolygon()
{
	//edges
	std::vector<Arr_Traits::Segment_2> segments;
	Triangulation::edges.clear();

	int count = 0;
	ez_edge edge;
	Polygon_2::Segment_2 s;
	for (Polygon_2::Edge_const_iterator ei = Triangulation::polygon.edges_begin(); ei != Triangulation::polygon.edges_end(); ++ei)
	{
		s = *ei;

		edge.source.x = s.source().x();
		edge.source.y = s.source().y();
		edge.target.x = s.target().x();
		edge.target.y = s.target().y();
		segments.push_back(s);
		Triangulation::gsegments.push_back(s);
		count++;
		edges.push_back(edge);
	}

	CGAL::insert_non_intersecting_curves(Triangulation::visibilityArrangement, segments.data(), segments.data() + count);
	CGAL::insert_non_intersecting_curves(Triangulation::testTriangulationArrangement, segments.data(), segments.data() + count);

	Triangulation::vertices.clear();
	ez_point point;
	Polygon_2::Point_2 p;
	for (Polygon_2::Vertex_const_iterator vi = Triangulation::polygon.vertices_begin(); vi != Triangulation::polygon.vertices_end(); ++vi)
	{
		p = *vi;

		point.x = p.x();
		point.y = p.y();

		vertices.push_back(point);
	}
}



void Triangulation::ThreeColorTraversalArrangement()
{
	//set all data to 0
	for (auto const& iterator : Triangulation::traversalArrangement.face_handles())
	{

		if (iterator->is_fictitious())
		{
			iterator->set_data(-1);
		}
		else {

			iterator->set_data(0);
		}
	}
	for (auto const& iterator : Triangulation::traversalArrangement.vertex_handles())
	{

		iterator->data().color = 0;
	}


	Arrangement_2::Face_iterator fi = Triangulation::traversalArrangement.faces_begin();
	// get a face
	while (fi->is_unbounded())
	{
		fi++;
	}

	Arrangement_2::Ccb_halfedge_circulator edge = fi->outer_ccb();

	ThreeColorVertices(edge);

}

void Triangulation::ThreeColorVertices(Arrangement_2::Ccb_halfedge_circulator edge)
{
	Arrangement_2::Vertex_handle v1 = edge->source();
	Arrangement_2::Vertex_handle v2 = edge->target();
	Arrangement_2::Vertex_handle v3 = edge->next()->target();

	int sum = v1->data().color + v2->data().color + v3->data().color;


	ez_point temp;

	if (sum == 0) {
		v1->data().color = 1;
		//v1->set_data(1);
		temp.x = v1->point().x();
		temp.y = v1->point().y();
		Triangulation::kcolor_1.push_back(temp);

		v2->data().color = 2;
		//v2->set_data(2);
		temp.x = v2->point().x();
		temp.y = v2->point().y();
		Triangulation::kcolor_2.push_back(temp);

		v3->data().color = 3;
		//v3->set_data(3);
		temp.x = v3->point().x();
		temp.y = v3->point().y();
		Triangulation::kcolor_3.push_back(temp);
	}
	else {
		if (v1->data().color == 0)
		{
			v1->data().color = (6 - sum);
			//v1->set_data(6 - sum);
			temp.x = v1->point().x();
			temp.y = v1->point().y();
			if (sum == 3)
			{
				Triangulation::kcolor_3.push_back(temp);
			}
			else if (sum == 4)
			{
				Triangulation::kcolor_2.push_back(temp);
			}
			else if (sum == 5)
			{
				Triangulation::kcolor_1.push_back(temp);
			}
		}
		else if (v2->data().color == 0)
		{
			v2->data().color = (6 - sum);
			//v2->set_data(6 - sum);
			temp.x = v2->point().x();
			temp.y = v2->point().y();
			if (sum == 3)
			{
				Triangulation::kcolor_3.push_back(temp);
			}
			else if (sum == 4)
			{
				Triangulation::kcolor_2.push_back(temp);
			}
			else if (sum == 5)
			{
				Triangulation::kcolor_1.push_back(temp);
			}
		}
		else if (v3->data().color == 0) {
			v3->data().color = (6 - sum);
			//v3->set_data(6 - sum);
			temp.x = v3->point().x();
			temp.y = v3->point().y();
			if (sum == 3)
			{
				Triangulation::kcolor_3.push_back(temp);
			}
			else if (sum == 4)
			{
				Triangulation::kcolor_2.push_back(temp);
			}
			else if (sum == 5)
			{
				Triangulation::kcolor_1.push_back(temp);
			}
		}
	}
	edge->face()->set_data(1);


	int i = 0;
	ez_edge e;
	Arrangement_2::Ccb_halfedge_circulator edge2;
	Arrangement_2::Vertex_handle v4, v5, v6;
	Point_2 s, t;
	while (i < 3)
	{

		if (!edge->twin()->face()->is_unbounded() && edge->twin()->face()->data() == 0)
		{
			edge2 = edge->twin()->face()->outer_ccb();
			v4 = edge2->source();
			v5 = edge2->target();
			v6 = edge2->next()->target();
			s = CGAL::centroid(v1->point(), v2->point(), v3->point());
			t = CGAL::centroid(v4->point(), v5->point(), v6->point());

			e.source.x = s.x();
			e.source.y = s.y();
			e.target.x = t.x();
			e.target.y = t.y();
			Triangulation::dualgraph_edges.push_back(e);

			ThreeColorVertices(edge2);
		}
		edge++;
		i++;
	}


}


void Triangulation::GenerateNew() {
	Triangulation::edges.clear();
	Triangulation::vertices.clear();
	Triangulation::monotone_edges.clear();
	Triangulation::triangulation_edges.clear();
	Triangulation::arrangement_edges.clear();
	Triangulation::visibility_vertices.clear();
	Triangulation::gsegments.clear();
	Triangulation::dualgraph_edges.clear();
	Triangulation::kcolor_1.clear();
	Triangulation::kcolor_2.clear();
	Triangulation::kcolor_3.clear();
	Triangulation::test_triangulation_edges.clear();

	Triangulation::polygon.clear();
	Triangulation::y_mono_polygons.clear();
	Triangulation::visibilityArrangement.clear();
	Triangulation::testTriangulationArrangement.clear();
	Triangulation::traversalArrangement.clear();
	Triangulation::constrainedTriangulation.clear();

	auto start_time = high_resolution_clock::now();
	
	CGAL::random_polygon_2(15, std::back_inserter(Triangulation::polygon),
		Point_generator(250));

	auto end_time = high_resolution_clock::now();
	auto duration_generation = duration_cast<milliseconds> (end_time - start_time).count();
	std::cout << "Runtime is: " << duration_generation << "\n";
	

	PopulateArrangementAndDrawInternalsFromPolygon();
	YMonotonePartitionPolygon2();
	//TriangulateYMonotonePartitions();
	CustomTriangulationImplementation();

	
	CGAL::insert_non_intersecting_curves(Triangulation::traversalArrangement, gsegments.data(), gsegments.data() + gsegments.size());
	InternalArrangement2Ez();
	ThreeColorTraversalArrangement();

}

double Triangulation::GenerateNew_Runtime() {
	auto start_time = high_resolution_clock::now();
	Triangulation::GenerateNew();
	auto end_time = high_resolution_clock::now();
	auto duration_generation = duration_cast<milliseconds > (end_time - start_time).count();
	return static_cast<double>(duration_generation);
}


void Triangulation::InternalArrangement2Ez()
{
	Triangulation::arrangement_edges.clear();
	ez_edge edge;
	//for (Arrangement_2::Edge_iterator ei = Triangulation::traversalArrangement.edges_begin(); ei != Triangulation::traversalArrangement.edges_end(); ++ei)
	for (Arrangement::Edge_const_iterator ei = Triangulation::visibilityArrangement.edges_begin(); ei != Triangulation::visibilityArrangement.edges_end(); ++ei)
	{
		edge.source.x = ei->source()->point().x();
		edge.source.y = ei->source()->point().y();
		edge.target.x = ei->target()->point().x();
		edge.target.y = ei->target()->point().y();

		arrangement_edges.push_back(edge);
	}
}


void Triangulation::YMonotonePartitionPolygon2()
{
	CGAL::y_monotone_partition_2(Triangulation::polygon.vertices_begin(),
		Triangulation::polygon.vertices_end(),
		std::back_inserter(Triangulation::y_mono_polygons));

	assert(CGAL::partition_is_valid_2(Triangulation::polygon.vertices_begin(),
		Triangulation::polygon.vertices_end(),
		Triangulation::y_mono_polygons.begin(),
		Triangulation::y_mono_polygons.end()));



	Triangulation::monotone_edges.clear();

	ez_edge edge;
	Polygon_2::Segment_2 s;
	std::vector<Polygon_2::Segment_2> seg_vec;

	for (const Polygon_2& poly : Triangulation::y_mono_polygons) {
		for (Polygon_2::Edge_const_iterator ei = poly.edges_begin(); ei != poly.edges_end(); ++ei)
		{
			s = *ei;
			if (Triangulation::polygon.has_on_bounded_side(CGAL::midpoint(s)))
			{
				auto it = std::find(seg_vec.begin(), seg_vec.end(), s);
				auto it2 = std::find(seg_vec.begin(), seg_vec.end(), s.opposite());

				if (it != seg_vec.end() || it2 != seg_vec.end()) {
					//std::cout << "Repeat" << std::endl;
				}
				else {
					//std::cout << "Adding one to seg_vec" << std::endl;
					seg_vec.push_back(s);
					Triangulation::gsegments.push_back(s);
					
				}
				edge.source.x = s.source().x();
				edge.source.y = s.source().y();
				edge.target.x = s.target().x();
				edge.target.y = s.target().y();

				//std::cout << "Adding one to mono_edges" << std::endl;
				monotone_edges.push_back(edge);

			}
		}
	}
	CGAL::insert_non_intersecting_curves(Triangulation::testTriangulationArrangement, seg_vec.data(), seg_vec.data() + seg_vec.size());
	//std::cout << Triangulation::testTriangulationArrangement.number_of_faces() << " FACES" << " UNBOUND: " << Triangulation::testTriangulationArrangement.number_of_unbounded_faces() << std::endl;
}

void Triangulation::addPointFromClick(int xP, int yP) {
	Triangulation::polygon.push_back(Point_2(xP, yP));
	
}

void Triangulation::clearPoly(){
	Triangulation::polygon.clear();
	Triangulation::vertices.clear();
	Triangulation::monotone_edges.clear();
	Triangulation::triangulation_edges.clear();
	Triangulation::arrangement_edges.clear();
	Triangulation::visibility_vertices.clear();
	Triangulation::gsegments.clear();
	Triangulation::dualgraph_edges.clear();
	Triangulation::kcolor_1.clear();
	Triangulation::kcolor_2.clear();
	Triangulation::kcolor_3.clear();
	Triangulation::test_triangulation_edges.clear();
	Triangulation::polygon.clear();
	Triangulation::y_mono_polygons.clear();
	Triangulation::visibilityArrangement.clear();
	Triangulation::testTriangulationArrangement.clear();
	Triangulation::traversalArrangement.clear();
	Triangulation::constrainedTriangulation.clear();

}

void Triangulation::publicPointPopulation() {
	//edges
	std::vector<Arr_Traits::Segment_2> segments;
	Triangulation::edges.clear();

	int count = 0;
	ez_edge edge;
	Polygon_2::Segment_2 s;
	for (Polygon_2::Edge_const_iterator ei = Triangulation::polygon.edges_begin(); ei != Triangulation::polygon.edges_end(); ++ei){
		s = *ei;

		edge.source.x = s.source().x();
		edge.source.y = s.source().y();
		edge.target.x = s.target().x();
		edge.target.y = s.target().y();
		segments.push_back(s);
		Triangulation::gsegments.push_back(s);
		count++;
		edges.push_back(edge);
	}

	CGAL::insert_non_intersecting_curves(Triangulation::visibilityArrangement, segments.data(), segments.data() + count);
	CGAL::insert_non_intersecting_curves(Triangulation::testTriangulationArrangement, segments.data(), segments.data() + count);

	Triangulation::vertices.clear();
	ez_point point;
	Polygon_2::Point_2 p;
	for (Polygon_2::Vertex_const_iterator vi = Triangulation::polygon.vertices_begin(); vi != Triangulation::polygon.vertices_end(); ++vi)
	{
		p = *vi;

		point.x = p.x();
		point.y = p.y();

		vertices.push_back(point);
	}

	YMonotonePartitionPolygon2();
	//TriangulateYMonotonePartitions();
	CustomTriangulationImplementation();

	CGAL::insert_non_intersecting_curves(Triangulation::traversalArrangement, gsegments.data(), gsegments.data() + gsegments.size());
	InternalArrangement2Ez();
	ThreeColorTraversalArrangement();

}

