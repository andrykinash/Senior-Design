#include "MainWindow.h"
#include "Triangulation.h"
#include "ModifiedGraphicsView.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QLabel>
#include <QtWidgets/QMenu>
#include <QRandomGenerator>
#include <QLineEdit>
#include <QGraphicsScene>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

array<int, 100000> xArray;
array<int, 100000> yArray;
array<int, 100000> xArrayIn;
array<int, 100000> yArrayIn;

void MainWindow::Clear() {

	for (auto& line : dual_graph) {
		scene->removeItem(line);
		delete line;
	}
	for (auto& line : lines) {
		scene->removeItem(line);
		delete line;
	}
	for (auto& point : points) {
		scene->removeItem(point);
		delete point;
	}
	for (auto& area : areas) {
		scene->removeItem(area);
		delete area;
	}

	for (auto& area : rightClickArea) {
		scene->removeItem(area);
		delete area;
	}
	rightClickArea.clear();

	for (auto& point : rightClickPoint) {
		scene->removeItem(point);
		delete point;
	}
	rightClickPoint.clear();
	dual_graph.clear();
	lines.clear();
	points.clear();
	areas.clear();
	
	drawn = false;
	looker = false;
	xArray.fill(-1);
	yArray.fill(-1);
	xArrayIn.fill(-1);
	yArrayIn.fill(-1);
	xyInc = 0;
	
	notifier->setText("");
	notifier->setStyleSheet("QLabel{color:black}");
	
	/*draw_3color_button->setDisabled(true);
	action1->setChecked(false);
	action1->setDisabled(true);
	action2->setChecked(false);
	action2->setDisabled(true);*/

	if (visibility == true) {

		visibility_poc_button->setDisabled(false);
	}
	else {

		visibility_poc_button->setDisabled(true);
	}

	triangulate_button->setDisabled(true);
	draw_3color_button->setDisabled(true);
	action1->setChecked(false);
	action1->setDisabled(true);
	action2->setChecked(false);
	action2->setDisabled(true);
}

void MainWindow::AddLine(double x1, double y1, double x2, double y2, QPen p, int z = 4)
{
	QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, p);
	line->setZValue(z);

	// keep track of all of our lines we have added
	lines.push_back(line);
}

void MainWindow::DrawDualGraph() {
	
	if (looker == true) {
		QPen _Pen;
		_Pen.setColor(Qt::darkGreen);
		_Pen.setWidth(5);
		_Pen.setCapStyle(Qt::RoundCap);
		
		for (auto const& line : triangulation.dualgraph_edges)
		{
			QGraphicsLineItem* lineD = scene->addLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen);
			lineD->setZValue(15);
			dual_graph.push_back(lineD);
		}
		
		drawn = true;
	} //end of looker if

}

void MainWindow::DrawTriangulationLines() {
	
	if (looker == true) {
		DrawMonotoneLines();
		
		QPen _Pen;
		_Pen.setColor(Qt::lightGray);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);
		
		for (auto const& line : triangulation.triangulation_edges)
		{
			AddLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen, 2);
		}
		
		drawn = true;
	}
}

void MainWindow::DrawRightClickArea(ez_point point)
{	
	if(looker == true){
		for (auto& area : rightClickArea) {
			scene->removeItem(area);
			delete area;
		}
		rightClickArea.clear();
		
		for (auto& point : rightClickPoint) {
			scene->removeItem(point);
			delete point;
		}
		
		rightClickPoint.clear();
		
		if (!triangulation.VisAtPoint(point))
		{
			return;
		}
		
		QPolygonF polygon;
		
		//draw area thing test
		for (auto const& point : triangulation.right_click_visibility)
		{
			polygon << QPointF(point.x, point.y);
		}
		
		QPen _Pen;
		_Pen.setColor(Qt::red);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);
		
		QBrush _Brush(Qt::Dense6Pattern);
		_Brush.setColor(Qt::red);
		
		QGraphicsPolygonItem* p = scene->addPolygon(polygon, _Pen, _Brush);
		p->setZValue(8);
		rightClickArea.push_back(p);
		
		//pen is the outline
		_Pen.setColor(Qt::red);
		_Pen.setWidth(2);
		int fatness = 6;
		
		QGraphicsRectItem* pointD = scene->addRect(QRectF(point.x - fatness / 2, point.y - fatness / 2, fatness, fatness), _Pen, _Brush);
		pointD->setZValue(10);
		//keep track of all of our points we have added
		rightClickPoint.push_back(pointD);
		
	}
}

QColor getRandomQtColor()
{
	const int colorCount = QColor::colorNames().count();
	const QString randomColorName = QColor::colorNames().at(QRandomGenerator::global()->bounded(colorCount));
	return QColor(randomColorName);
}

void MainWindow::DrawArrangementLines() {
	if (looker == true) {
		visibility = true;
		Clear();
		visibility = false;
		
		QPen _Pen;
		_Pen.setColor(Qt::black);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);
		
		for (auto const& line : triangulation.arrangement_edges)
		{
			AddLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen, 9);
		}
	

		std::vector<QColor> colors = { Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta };

		int colorIndex = 0;
		for (auto const& point : ColorCheck(LeastCheck()))
		{
			triangulation.CalculateArrangementVisibilityAtPoint(point);

			QPolygonF polygon;

			// assign a different color to each polygon
			QColor randomColor = colors[colorIndex % colors.size()];
			colorIndex++;

			for (auto const& point : triangulation.visibility_vertices)
			{
				polygon << QPointF(point.x, point.y);
			}

			_Pen.setColor(randomColor);
			_Pen.setWidth(3);
			_Pen.setCapStyle(Qt::RoundCap);

			// create a radial gradient brush with more color stops
			QRadialGradient gradient(QPointF(point.x, point.y), 100);
			gradient.setColorAt(0, randomColor);
			gradient.setColorAt(0.3, QColor(randomColor.red(), randomColor.green(), randomColor.blue(), 75));
			gradient.setColorAt(0.6, QColor(randomColor.red(), randomColor.green(), randomColor.blue(), 50));
			gradient.setColorAt(1, QColor(randomColor.red(), randomColor.green(), randomColor.blue(), 25));


			QBrush _Brush(gradient);

			QGraphicsPolygonItem* p = scene->addPolygon(polygon, _Pen, _Brush);
			p->setZValue(8);
			areas.push_back(p);

			//redraw the point
			_Brush.setColor(randomColor);
			_Brush.setStyle(Qt::SolidPattern);

			//pen is the outline
			_Pen.setColor(randomColor);
			_Pen.setWidth(2);
			int fatness = 6;

			QGraphicsRectItem* Zpoint = scene->addRect(QRectF(point.x - fatness / 2, point.y - fatness / 2, fatness, fatness), _Pen, _Brush);
			Zpoint->setZValue(10);
			//keep track of all of our points we have added
			points.push_back(Zpoint);
		}
		
		/*
		triangulation.CalculateArrangementVisibilityAtPoint(triangulation.vertices[j]);
		
		QPolygonF polygon;
		
		//draw area thing test
		for (auto const& point : triangulation.visibility_vertices)
		{
			polygon << QPointF(point.x, point.y);
		}
		
		_Pen.setColor(Qt::red);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);
	
		QBrush _Brush(Qt::Dense6Pattern);
		_Brush.setColor(Qt::red);
		
		QGraphicsPolygonItem* p = scene->addPolygon(polygon, _Pen, _Brush);
		p->setZValue(8);
		areas.push_back(p);
		
		//redraw the point
		_Brush.setColor(Qt::red);
		_Brush.setStyle(Qt::SolidPattern);
		
		//pen is the outline
		_Pen.setColor(Qt::red);
		_Pen.setWidth(2);
		int fatness = 6;
		
		QGraphicsRectItem* point = scene->addRect(QRectF(triangulation.vertices[j].x - fatness / 2, triangulation.vertices[j].y - fatness / 2, fatness, fatness), _Pen, _Brush);
		point->setZValue(10);
		//keep track of all of our points we have added
		points.push_back(point);
		*/
		
		drawn = true;
		
	}

}

void MainWindow::DrawMonotoneLines() {
	
	if (looker == true) {
		QPen _Pen;
		_Pen.setColor(Qt::darkYellow);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);
		
		for (auto const& line : triangulation.monotone_edges)
		{
			AddLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen, 3);
		}
		
		drawn = true;
	}
}

void MainWindow::DrawPolygon()
{
	Clear();

	if (showDG)
	{
		DrawDualGraph();
	}

	QPen _Pen;
	_Pen.setColor(Qt::black);
	_Pen.setWidth(2);

	for (auto const& line : triangulation.edges)
	{
		AddLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen);
	}

	//brush is the fill
	QBrush _Brush;
	_Brush.setColor(Qt::cyan);
	_Brush.setStyle(Qt::SolidPattern);

	//pen is the outline
	_Pen.setColor(Qt::blue);
	_Pen.setWidth(2);
	int fatness = 6;
	// try to add a tiny square at each first line coordinate?
	for (auto& line : lines) {
		QGraphicsRectItem* point = scene->addRect(QRectF(line->line().x1() - fatness / 2, line->line().y1() - fatness / 2, fatness, fatness), _Pen, _Brush);
		point->setZValue(5);
		//keep track of all of our points we have added
		points.push_back(point);
	}

	drawn = true;
	looker = true;
	
	triangulate_button->setDisabled(false);
	draw_3color_button->setDisabled(false);
	visibility_poc_button->setDisabled(false);
	action1->setChecked(false);
	action1->setDisabled(false);
	action2->setChecked(false);
	action2->setDisabled(false);

	QRectF boundingRect = scene->itemsBoundingRect();
	boundingRect.adjust(-20, -20, 20, 20); // add a 20-pixel margin around the items
	scene->setSceneRect(boundingRect);
}

void MainWindow::GenerateNew() {
	
	Clear();
	triangulation.GenerateNew();


	DrawPolygon();
	if (showDG)
	{
		DrawDualGraph();
	}
	
	LeastCameras(ColorCheck(LeastCheck()));

}

void MainWindow::DrawThreeColorVertices()
{
	if (looker == true) {
		QBrush _Brush;
		_Brush.setStyle(Qt::SolidPattern);
		QPen _Pen;
		_Pen.setWidth(2);
		int fatness = 6;
		
		for (auto const& p : triangulation.kcolor_1)
		{
			_Brush.setColor(Qt::red);
			_Pen.setColor(Qt::red);
			QGraphicsRectItem* point = scene->addRect(QRectF(p.x - fatness / 2, p.y - fatness / 2, fatness, fatness), _Pen, _Brush);
			point->setZValue(20);
			points.push_back(point);
		}
		for (auto const& p : triangulation.kcolor_2)
		{
			_Brush.setColor(Qt::green);
			_Pen.setColor(Qt::green);
			QGraphicsRectItem* point = scene->addRect(QRectF(p.x - fatness / 2, p.y - fatness / 2, fatness, fatness), _Pen, _Brush);
			point->setZValue(20);
			points.push_back(point);
		}
		for (auto const& p : triangulation.kcolor_3)
		{
			_Brush.setColor(Qt::black);
			_Pen.setColor(Qt::black);
			QGraphicsRectItem* point = scene->addRect(QRectF(p.x - fatness / 2, p.y - fatness / 2, fatness, fatness), _Pen, _Brush);
			point->setZValue(20);
			points.push_back(point);
		}
		
		drawn = true;
		
		LeastCheck();
	}
}

void MainWindow::LeastCameras(std::vector<ez_point> color) { //This function displays the needed cameras only

	QBrush _Brush;
	_Brush.setStyle(Qt::SolidPattern);
	QPen _Pen;
	_Pen.setWidth(2);
	int fatness = 6;

	Qt::GlobalColor leastColor = LeastCheck();

	for (auto const& p : color) {
		_Brush.setColor(leastColor);
		_Pen.setColor(leastColor);
		QGraphicsRectItem* point = scene->addRect(QRectF(p.x - fatness / 2, p.y - fatness / 2, fatness, fatness), _Pen, _Brush);
		point->setZValue(20);
		points.push_back(point);
	}
}

Qt::GlobalColor MainWindow::LeastCheck() { //This function changes the label to display the number of cameras needed while also returning the color
	
	if ((triangulation.kcolor_1.size() < triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() < triangulation.kcolor_2.size())) { //If red has the least

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_1.size()));
		notifier->setStyleSheet("QLabel{color:red}");
		
		return Qt::red;
	}
	else if ((triangulation.kcolor_1.size() > triangulation.kcolor_3.size()) && (triangulation.kcolor_3.size() < triangulation.kcolor_2.size())) { //If black has the least

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_3.size()));
		notifier->setStyleSheet("QLabel{color:black}");

		return Qt::black;
	}
	else if ((triangulation.kcolor_2.size() < triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() > triangulation.kcolor_2.size())) { //If green has the least

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_2.size()));
		notifier->setStyleSheet("QLabel{color:green}");

		return Qt::green;
	}
	else if ((triangulation.kcolor_2.size() == triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() == triangulation.kcolor_2.size())) { //If all are equal

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_1.size()));
		notifier->setStyleSheet("QLabel{color:red}");

		return Qt::red;
	}
	else if ((triangulation.kcolor_2.size() == triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() > triangulation.kcolor_2.size())) { //If green and black are equal and red is more

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_2.size()));
		notifier->setStyleSheet("QLabel{color:green}");

		return Qt::green;
	}
	else if ((triangulation.kcolor_2.size() < triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() == triangulation.kcolor_2.size())) { //If red and green are equal and black is more

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_1.size()));
		notifier->setStyleSheet("QLabel{color:red}");

		return Qt::red;
	}
	else if ((triangulation.kcolor_2.size() > triangulation.kcolor_3.size()) && (triangulation.kcolor_1.size() == triangulation.kcolor_3.size())) { //If red and black are equal and green is more

		notifier->setText(QString("%1 Cameras Needed!").arg(triangulation.kcolor_1.size()));
		notifier->setStyleSheet("QLabel{color:red}");

		return Qt::red;
	}

	return Qt::lightGray; //Error case (should never reach this)
}

std::vector<ez_point> MainWindow::ColorCheck(Qt::GlobalColor checkMe) {
	
	if (checkMe == Qt::red) {
		
		return triangulation.kcolor_1;
	}
	else if (checkMe == Qt::green) {

		return triangulation.kcolor_2;
	}
	else if (checkMe == Qt::black) {

		return triangulation.kcolor_3;
	}

	return triangulation.kcolor_1; //Should never reach here
}

void MainWindow::TestDrawTriang() {
	if (looker == true) {
		DrawMonotoneLines();
		QPen _Pen;
		_Pen.setColor(Qt::lightGray);
		_Pen.setWidth(2);
		_Pen.setCapStyle(Qt::RoundCap);

		for (auto const& line : triangulation.test_triangulation_edges)
		{
			AddLine(line.source.x, line.source.y, line.target.x, line.target.y, _Pen, 2);
		}
	}
}

void MainWindow::InstructionFunction() {

	// Create a new QDialog window
	QDialog* instructionDialog = new QDialog(this);
	instructionDialog->setWindowTitle("Art Gallery Instructions");

	// Create QLabel widgets to display the instructions text
	QLabel* instructionsGoal = new QLabel(instructionDialog);
	instructionsGoal->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsGoal->setText("This program can be used to find the minimum number of cameras necessary to guard any given room that\ncan be represented by a simple polygon. To create a polygon, left click on the drawing area. Each successive\nleft click will result in a new vertex for the polygon. To draw the final vertex, right click instead of\nleft clicking and the shape will complete. If the input shape is invalid, you will be notified and the shape\nwill not be drawn. Otherwise, the shape will be finalized and the minimum number of cameras will be displayed\nboth on the shape and in the label on the right side of the button bar.");
	instructionsGoal->setMaximumHeight(120);

	QLabel* instructionsButtons = new QLabel(instructionDialog);
	instructionsButtons->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsButtons->setText("Instructions - Shows program instructions.\nProvide Vertex List - Allows you to enter in points by providing specific coordinates as opposed to clicking. First, the\nuser must enter how many vertices the shape will have and then they can enter the direct coordinates of each point.\nCreate Random Polygon - Generates a new random polygon.\nClear View - Clears canvas area of any vertices, edges, and polygons. Also clears the notification label.\nRedraw Polygon - Redraws the edges of the polygon and removes any additional visuals if present.\nTriangulate - Shows all lines made in order to triangulate the input polygon. Gold lines represent lines necessary\nto split the polygon into y-monotone pieces whereas gray lines are typical triangulation lines.\nDraw 3 Color - Displays all vertices colored in a way to have each triangle inside the shape have three unique\ncolors. This process will result in three sets of cameras of which the smallest is the minimum number of cameras.\nCamera Visibility - Displays the visibility of all cameras needed to cover the area at once. In order to use the Triangulate,\nDraw 3 Color, or Draw Dual Graph buttons again, please click Redraw Polygon. The user can check the visibility of any\ngiven point inside the polygon by right clicking within the shape after it is completed.");
	instructionsButtons->setMaximumHeight(220);

	QLabel* instructionsOptions = new QLabel(instructionDialog);
	instructionsOptions->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsOptions->setText("Show Dual Graph - Displays a path made through the center of each triangle made during triangulation. This\ncan visualize where a camera can see from inside a given triangle.");

	//Create a QFont specific to the headings
	QFont headFont("Arial", 10, QFont::Bold);

	//Create QLabels for the headings
	QLabel* topHeading = new QLabel(instructionDialog);
	topHeading->setText("The Goal:");
	
	QLabel* lowHeading = new QLabel(instructionDialog);
	lowHeading->setText("The Buttons:");

	QLabel* optionHeading = new QLabel(instructionDialog);
	optionHeading->setText("The Options:");

	topHeading->setFont(headFont);
	topHeading->setMaximumHeight(20);
	lowHeading->setFont(headFont);
	lowHeading->setMaximumHeight(20);
	optionHeading->setFont(headFont);
	optionHeading->setMaximumHeight(20);

	// Create a QVBoxLayout for the dialog and add the instructions labels to it
	QVBoxLayout* dialogLayout = new QVBoxLayout(instructionDialog);
	dialogLayout->addWidget(topHeading);
	dialogLayout->addWidget(instructionsGoal);
	dialogLayout->addWidget(lowHeading);
	dialogLayout->addWidget(instructionsButtons);
	dialogLayout->addWidget(optionHeading);
	dialogLayout->addWidget(instructionsOptions);

	// Resize the dialog window and show it
	instructionDialog->resize(600, 400);
	instructionDialog->exec();
}

void onComboBoxIndexChanged(int index)
{
	QString selectedOption = QString::number(index + 1);
	/*
	QMessageBox::information(nullptr, "Selection Changed",
		"You selected option " + selectedOption);
		*/
}

void MainWindow::ToggleDualGraph(bool checked) {
	if (checked)
	{
		showDG = true;
		DrawDualGraph();
	}
	else
	{
		showDG = false;
		for (auto& line : dual_graph) {
			scene->removeItem(line);
			delete line;
		}
		dual_graph.clear();
	}
}



void MainWindow::onAction2Triggered() {
	// Create a new QDialog window
	QDialog* instructionDialog = new QDialog(this);
	instructionDialog->setWindowTitle("Run Times for functions");

	// Create QLabel widgets to display the instructions text
	QLabel* instructionsGoal = new QLabel(instructionDialog);
	instructionsGoal->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsGoal->setText(QString("this will include runtime of polygon generation: %1").arg(QString::number(triangulation.GenerateNew_Runtime(), 'f', 50)));
	instructionsGoal->setMaximumHeight(120);

	QLabel* instructionsButtons = new QLabel(instructionDialog);
	instructionsButtons->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsButtons->setText("This will include runtime of triangualtion function");
	instructionsButtons->setMaximumHeight(220);

	QLabel* instructionsOptions = new QLabel(instructionDialog);
	instructionsOptions->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	instructionsOptions->setText("This will include runtime of 3 color function");

	//Create a QFont specific to the headings
	QFont headFont("Arial", 10, QFont::Bold);

	//Create QLabels for the headings
	QLabel* topHeading = new QLabel(instructionDialog);
	topHeading->setText("Polygon generation runtimes:");

	QLabel* lowHeading = new QLabel(instructionDialog);
	lowHeading->setText("Triangulation runtimes");

	QLabel* optionHeading = new QLabel(instructionDialog);
	optionHeading->setText("3 color runtimes");

	topHeading->setFont(headFont);
	topHeading->setMaximumHeight(20);
	lowHeading->setFont(headFont);
	lowHeading->setMaximumHeight(20);
	optionHeading->setFont(headFont);
	optionHeading->setMaximumHeight(20);

	// Create a QVBoxLayout for the dialog and add the instructions labels to it
	QVBoxLayout* dialogLayout = new QVBoxLayout(instructionDialog);
	dialogLayout->addWidget(topHeading);
	dialogLayout->addWidget(instructionsGoal);
	dialogLayout->addWidget(lowHeading);
	dialogLayout->addWidget(instructionsButtons);
	dialogLayout->addWidget(optionHeading);
	dialogLayout->addWidget(instructionsOptions);

	// Resize the dialog window and show it
	instructionDialog->resize(600, 400);
	instructionDialog->exec();
}

void MainWindow::onAction3Triggered() {
}

void MainWindow::mousePressEvent(QMouseEvent *click, ez_point inPoint) {
	if (xyInc == 0) { //reinitialize arrays
		xArray.fill(-1);
		yArray.fill(-1);

	}

	QPen _Pen;
	_Pen.setWidth(2);
	int fatness = 6;
	_Pen.setColor(Qt::black);

	QPoint clicked = click->pos();
	int holdX;
	int holdY;

	if (drawn == false) { //no shape is currently on the canvas
		if (click->button() == Qt::LeftButton) { //if it's a left click

			holdX = clicked.x(); //window coordinates just in case
			holdY = clicked.y();
			
			xArray[xyInc] = inPoint.x;
			yArray[xyInc] = inPoint.y;
			xyInc++;

			QBrush _Brush;
			_Brush.setStyle(Qt::SolidPattern);
			QPen _Pen;
			_Pen.setWidth(1);
			int fatness = 6;

			_Brush.setColor(Qt::magenta);
			_Pen.setColor(Qt::magenta);
			QGraphicsRectItem* point = scene->addRect(QRectF(inPoint.x - fatness/2, inPoint.y - fatness/2, fatness, fatness), _Pen, _Brush);

			//ensures these points will delete when cleared
			point->setZValue(5);
			points.push_back(point);

			if (xyInc > 1) {
				AddLine(xArray[xyInc - 2], yArray[xyInc - 2], xArray[xyInc - 1], yArray[xyInc - 1], _Pen, 4);

			}

		} //end of left click if

		if (click->button() == Qt::RightButton) { //if it's a right click on the canvas
			holdX = clicked.x(); //window coordinates just in case
			holdY = clicked.y();

			if (xyInc >= 2) { //triangulation is possible
				xArray[xyInc] = inPoint.x;
				yArray[xyInc] = inPoint.y;
				xyInc++;

				int currentPos = 0;
				
				triangulation.clearPoly(); //clear out old stuff

				while (xArray[currentPos] != -1) { //Catalog points and add lines loop
					triangulation.addPointFromClick(xArray[currentPos], yArray[currentPos]);

					if (xArray[currentPos + 1] != -1) { //not the last point
						AddLine(xArray[currentPos], yArray[currentPos], xArray[currentPos + 1], yArray[currentPos + 1], _Pen, 4); //add line

					} else { //the last point
						AddLine(xArray[currentPos], yArray[currentPos], xArray[0], yArray[0], _Pen, 4); //connect to point 0

					}

					currentPos++;

				} //end of while loop
				
				if (triangulation.polygon.is_simple() != true) { //incorrect shape
					Clear();
					notifier->setText("Invalid Shape, Try Again.");
					notifier->setStyleSheet("QLabel{color:darkRed}");

				} else { //correct shape
					if (triangulation.polygon.is_clockwise_oriented()) { //checks if the polygon is oriented properly
						triangulation.polygon.reverse_orientation();

					}

					triangulation.publicPointPopulation(); //add in all triangualation information

					QBrush _Brush;
					_Brush.setColor(Qt::cyan);
					_Brush.setStyle(Qt::SolidPattern);

					_Pen.setColor(Qt::blue);
					_Pen.setWidth(2);
					int fatness = 6;

					for (auto& line : lines) {
						QGraphicsRectItem* point = scene->addRect(QRectF(line->line().x1() - fatness / 2, line->line().y1() - fatness / 2, fatness, fatness), _Pen, _Brush);
						point->setZValue(5);
						//keep track of all of our points we have added
						points.push_back(point);

					}

					drawn = true;
					looker = true;

					triangulate_button->setDisabled(false);
					draw_3color_button->setDisabled(false);
					visibility_poc_button->setDisabled(false);
					action1->setChecked(false);
					action1->setDisabled(false);
					action2->setChecked(false);
					action2->setDisabled(false);

					LeastCameras(ColorCheck(LeastCheck()));

					QRectF boundingRect = scene->itemsBoundingRect();
					boundingRect.adjust(-20, -20, 20, 20); // add a 20-pixel margin around the items
					scene->setSceneRect(boundingRect);


				}
			} else { //not yet triangulatable
				xArray[xyInc] = inPoint.x;
				yArray[xyInc] = inPoint.y;
				xyInc++;

				QBrush _Brush;
				_Brush.setStyle(Qt::SolidPattern);
				QPen _Pen;
				_Pen.setWidth(1);
				int fatness = 6;

				_Brush.setColor(Qt::magenta);
				_Pen.setColor(Qt::magenta);
				QGraphicsRectItem* point = scene->addRect(QRectF(inPoint.x - fatness / 2, inPoint.y - fatness / 2, fatness, fatness), _Pen, _Brush);

				//ensures these points will delete when cleared
				point->setZValue(5);
				points.push_back(point);

				if (xyInc > 1) {
					AddLine(xArray[xyInc - 2], yArray[xyInc - 2], xArray[xyInc - 1], yArray[xyInc - 1], _Pen, 4);

				}

			}

		} //end of right click if
	} //end of drawn-limited if statement
}

void MainWindow::InputListWindow() {
	vertInc = 0;
	inVal = 0;

	inputListW = new QDialog(this);
	inputListW->setWindowTitle("Input List of Vertices");

	ilMainText = new QLabel(inputListW);
	ilMainText->setText(announcement);
	ilMainText->setAlignment(Qt::AlignTop | Qt::AlignCenter);

	numVert = new QLineEdit;
	numVert->setPlaceholderText("Number of vertices");
	numVert->setAlignment(Qt::AlignCenter);

	numVertY = new QLineEdit;
	numVertY->setAlignment(Qt::AlignCenter);
	numVertY->setEnabled(false);

	inputListButton = new QPushButton("Submit value");
	inputListButton->setMinimumWidth(200);
	connect(inputListButton, &QPushButton::released, this, &MainWindow::InputListClick);

	holder = new QVBoxLayout(inputListW);

	holder->addWidget(ilMainText);
	holder->addWidget(numVert);
	holder->addWidget(numVertY);
	holder->addWidget(inputListButton);

	announcement = "Please input the amount of vertices your planned gallery is.";

	inputListW->resize(200, 150);
	inputListW->exec();

}

void MainWindow::InputListClick() {
	QString retextL;

	if (vertInc == 0) {
		Clear();

		inVal = numVert->text().toInt();
		
		if (inVal < 3) { //invalid input
			announcement = "This amount of vertices is invalid, please try again.";
			ilMainText->setText(announcement);
			numVert->clear();
			announcement = "Please input the amount of vertices your planned gallery is.";

		} else {
			numVertY->setEnabled(true);
			numVert->setPlaceholderText("X");
			numVertY->setPlaceholderText("Y");

			vertInc++;

			retextL = QString("Input vertex #%1").arg(vertInc);
			numVert->clear();
			numVertY->clear();
			ilMainText->setText(retextL);

		}

	} else { //not first time clicking on button
		if (vertInc < inVal) {
			xArrayIn[vertInc - 1] = numVert->text().toInt();
			yArrayIn[vertInc - 1] = numVertY->text().toInt();

			vertInc++;
			retextL = QString("Input vertex #%1").arg(vertInc);

			numVert->clear();
			numVertY->clear();
			ilMainText->setText(retextL);

		} else { //check, triangulate and end if applicable
			xArrayIn[vertInc - 1] = numVert->text().toInt();
			yArrayIn[vertInc - 1] = numVertY->text().toInt();

			int currentPos = 0;
			
			QPen _Pen;
			_Pen.setColor(Qt::black);
			_Pen.setWidth(2);
			int fatness = 6;

			triangulation.clearPoly(); //clear out old stuff

			while (xArrayIn[currentPos] != -1) { //Catalog points and add lines loop
				triangulation.addPointFromClick(xArrayIn[currentPos], yArrayIn[currentPos]);

				if (xArrayIn[currentPos + 1] != -1) { //not the last point
					AddLine(xArrayIn[currentPos], yArrayIn[currentPos], xArrayIn[currentPos + 1], yArrayIn[currentPos + 1], _Pen, 4); //add line

				}
				else { //the last point
					AddLine(xArrayIn[currentPos], yArrayIn[currentPos], xArrayIn[0], yArrayIn[0], _Pen, 4); //connect to point 0

				}

				currentPos++;

			} //end of while loop

			if (triangulation.polygon.is_simple() != true) { //incorrect shape
				Clear();
				inputListW->accept();

				announcement = "Invalid input, please start the input process again.";
				InputListWindow();

			} else { //correct shape
				if (triangulation.polygon.is_clockwise_oriented()) { //checks if the polygon is oriented properly
					triangulation.polygon.reverse_orientation();

				}

				triangulation.publicPointPopulation(); //add in all triangualation information

				QBrush _Brush;
				_Brush.setColor(Qt::cyan);
				_Brush.setStyle(Qt::SolidPattern);

				_Pen.setColor(Qt::blue);
				_Pen.setWidth(2);
				int fatness = 6;

				for (auto& line : lines) {
					QGraphicsRectItem* point = scene->addRect(QRectF(line->line().x1() - fatness / 2, line->line().y1() - fatness / 2, fatness, fatness), _Pen, _Brush);
					point->setZValue(5);
					//keep track of all of our points we have added
					points.push_back(point);

				}

				drawn = true;
				looker = true;

				triangulate_button->setDisabled(false);
				draw_3color_button->setDisabled(false);
				visibility_poc_button->setDisabled(false);
				action1->setChecked(false);
				action1->setDisabled(false);
				action2->setChecked(false);
				action2->setDisabled(false);

				LeastCameras(ColorCheck(LeastCheck()));
				inputListW->accept();

			} //end of triangulation if

		} //end of main if

	}
}

MainWindow::MainWindow(QWidget* parent) : QDialog(parent)
{	
	announcement = "Please input the amount of vertices your planned gallery is.";

	srand(time(nullptr));

	setWindowTitle("Art Gallery");

	resize(800, 600);

	mainlayout = new QVBoxLayout;

	//menu

	QMenu* menu = new QMenu(this);

	action1 = new QAction("Show Dual Graph", this);
	connect(action1, &QAction::toggled, this, &MainWindow::ToggleDualGraph);
	menu->addAction(action1);

	action2 = new QAction("Show runtimes", this);
	connect(action2, &QAction::toggled, this, &MainWindow::onAction2Triggered);
	menu->addAction(action2);

	action3 = new QAction("Graphed Runtimes", this);
	connect(action3, &QAction::toggled, this, &MainWindow::onAction3Triggered);
	menu->addAction(action3);
	

	action1->setCheckable(true);
	action2->setCheckable(true);
	action3->setCheckable(true);

	//add buttons
	bottomButtonRow = new QHBoxLayout();
	auto* topButtonRow = new QHBoxLayout();

	auto* provide_vertex_list_button = new QPushButton("Provide Vertex List");
	connect(provide_vertex_list_button, &QPushButton::released, this, &MainWindow::InputListWindow);

	auto* draw_polygon_button = new QPushButton("Redraw Polygon");
	connect(draw_polygon_button, &QPushButton::released, this, &MainWindow::DrawPolygon);

	auto* clear_view_button = new QPushButton("Clear View");
	connect(clear_view_button, &QPushButton::released, this, &MainWindow::Clear);

	auto* instructions_button = new QPushButton("Instructions");
	connect(instructions_button, &QPushButton::released, this, &MainWindow::InstructionFunction);

	triangulate_button = new QPushButton("Triangulate");
	connect(triangulate_button, &QPushButton::released, this, &MainWindow::TestDrawTriang);

	/*
	auto* btn6 = new QPushButton("Draw Dual Graph");
	connect(btn6, &QPushButton::released, this, &MainWindow::DrawDualGraph);
	*/

	draw_3color_button = new QPushButton("Draw 3 Color");
	connect(draw_3color_button, &QPushButton::released, this, &MainWindow::DrawThreeColorVertices);

	visibility_poc_button = new QPushButton("Camera Visibility");
	connect(visibility_poc_button, &QPushButton::released, this, &MainWindow::DrawArrangementLines);

	auto* create_new_polygon_button = new QPushButton("Generate Random Polygon");
	connect(create_new_polygon_button, &QPushButton::released, this, &MainWindow::GenerateNew);

	//auto* wip_triangulation_button = new QPushButton("WIP Triangulation");
	//connect(wip_triangulation_button, &QPushButton::released, this, &MainWindow::TestDrawTriang);

	//auto* triangulation_dropdown_selector = new QComboBox;
	//triangulation_dropdown_selector->addItem("Y-Monotone Triangulation");
	//triangulation_dropdown_selector->addItem("CGAL Triangulation");
	//connect(triangulation_dropdown_selector, QOverload<int>::of(&QComboBox::currentIndexChanged),
	//	&onComboBoxIndexChanged);

	QPushButton* menuButton = new QPushButton("Options", this);
	menuButton->setMenu(menu);

	notifier = new QLabel(this);
	notifier->setText("");
	notifier->setFrameStyle(QFrame::Panel | QFrame::Raised);
	notifier->setAlignment(Qt::AlignCenter | Qt::AlignCenter);

	topButtonRow->addWidget(instructions_button);
	topButtonRow->addWidget(provide_vertex_list_button);
	topButtonRow->addWidget(create_new_polygon_button);
	topButtonRow->addWidget(clear_view_button);
	topButtonRow->addWidget(menuButton);

	bottomButtonRow->addWidget(draw_polygon_button);
	bottomButtonRow->addWidget(triangulate_button);
	bottomButtonRow->addWidget(draw_3color_button);
	//bottomButtonRow->addWidget(wip_triangulation_button);
	bottomButtonRow->addWidget(visibility_poc_button);
	//bottomButtonRow->addWidget(triangulation_dropdown_selector);
	bottomButtonRow->addWidget(notifier);

	mainlayout->addLayout(topButtonRow);
	mainlayout->addLayout(bottomButtonRow);

	setLayout(mainlayout);

	//view = new QGraphicsView;
	view = new ModifiedGraphicsView(nullptr, this);
	scene = new QGraphicsScene(this);
	view->setScene(scene);

	mainlayout->addWidget(view);

	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	view->setDragMode(QGraphicsView::ScrollHandDrag);
	view->scale(1, -1);

	view->show();
	DrawPolygon();
}
