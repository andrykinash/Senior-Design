#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include "ez.h"
#include "Triangulation.h"

class MainWindow : public QDialog {
public:
	explicit MainWindow(QWidget* parent = nullptr);
	void DrawRightClickArea(ez_point p);
	void mousePressEvent(QMouseEvent* click, ez_point inPoint);
	
private:
	bool showDG = false;
	QGraphicsView* view;
	QGraphicsScene* scene;
	QVBoxLayout* mainlayout;
	QHBoxLayout* bottomButtonRow;
	QLabel* notifier;
	
	int xyInc = 0;
	bool drawn = false; //boolean to prevent being able to draw while another shape exists
	bool looker = false; //boolean to prevent seeing a previous shape's things while blank
	bool visibility = false; //boolean to ensure some things do not disable when clicking the visibility button

	QDialog* inputListW;
	QLabel* ilMainText;
	QLineEdit* numVert;
	QLineEdit* numVertY;
	QVBoxLayout* holder;
	QPushButton* inputListButton;
	int vertInc;
	int inVal;
	QString announcement;
	
	std::list<QGraphicsLineItem*> dual_graph;
	std::list<QGraphicsLineItem*> lines;
	std::list<QGraphicsRectItem*> points;
	std::list<QGraphicsPolygonItem*> areas;

	QPushButton* triangulate_button;
	QPushButton* draw_3color_button;
	QPushButton* visibility_poc_button;
	QAction* action1;
	QAction* action2;
	QAction* action3;

	std::list<QGraphicsPolygonItem*> rightClickArea;
	std::list<QGraphicsRectItem*> rightClickPoint;
	Triangulation triangulation;
	
	double myfunction();
	void AddLine(double x1, double y1, double x2, double y2, QPen p, int z);
	void DrawTriangulationLines();
	void DrawMonotoneLines();
	void DrawArrangementLines();
	void DrawPolygon();
	void Clear();
	void DrawDualGraph();
	void DrawThreeColorVertices();
	void GenerateNew();
	void InstructionFunction();
	void TestDrawTriang();
	void onAction2Triggered();
	void onAction3Triggered();
	void LeastCameras(std::vector<ez_point> color);
	void InputListWindow();
	void InputListClick();
	
	Qt::GlobalColor LeastCheck(); //Returns the color with the least number of cameras
	std::vector<ez_point> ColorCheck(Qt::GlobalColor checkMe); //Returns the points associated with a given color
private Q_SLOTS:
	void ToggleDualGraph(bool checked);
};
