#include "MainWindowConstrctor.h"
#include <QApplication>
#include <QMainWindow>
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

int amain(int argc, char** argv)
{
	QApplication::setColorSpec(QApplication::CustomColor);
	QApplication app(argc,argv);
	glutInit(&argc,argv);
	QMainWindow mainWin;
	mainWin.resize(640,480);
	MainWindowConstructor constructor(mainWin);
	constructor.construct();
	mainWin.show();
	return app.exec();
}
