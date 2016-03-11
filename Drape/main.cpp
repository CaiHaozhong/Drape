#include "MainWindowConstrctor.h"
#include <QApplication>
#include "MainWindow.h"
#include "arthurwidgets.h"
#include <QMenuBar>
// void readMesh(const QString& fileName)
// {
// 	_mesh.request_vertex_normals();
// 	OpenMesh::IO::Options opt;
// 	if(!OpenMesh::IO::read_mesh(_mesh,fileName.toStdString(),opt))
// 	{
// 		printf("Cannot read mesh from file\n");
// 	}
// 	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
// 	{
// 		// we need face normals to update the vertex normals
// 		_mesh.request_face_normals();
// 		// let the mesh update the normals
// 		_mesh.update_normals();
// 		// dispose the face normals, as we don't need them anymore
// 		_mesh.release_face_normals();
// 	}
// 
// 	_mesh.add_property(_geo);
// }

int main(int argc, char** argv)
{
	QApplication::setColorSpec(QApplication::CustomColor);
	QApplication app(argc,argv);
	glutInit(&argc,argv);
	MainWindow mainWin;
	mainWin.resize(640,480);
	/* …Ë÷√∑Á∏Ò 
	QStyle* style = new ArthurStyle;
	mainWin.setStyle(style);	
	QList<QWidget *> widgets = mainWin.findChildren<QWidget *>();
	foreach (QWidget *w, widgets)
		w->setStyle(style);*/
	mainWin.show();
	
	return app.exec();
}
