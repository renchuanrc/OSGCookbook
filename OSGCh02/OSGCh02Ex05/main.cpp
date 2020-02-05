#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/ClipNode>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>

#include <osgUtil/Optimizer>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>

#include <osgSim/OverlayNode>

#include <osgText/Font>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <iostream>
#include "Common.h"
#include "PickHandler.h"


int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFiles(arguments);
	if (!scene) scene = osgDB::readNodeFile("cessna.osg");
	osg::ref_ptr<osg::Node> refNode = osgDB::readNodeFile("cow.osg");

	osg::ComputeBoundsVisitor cbv;
	scene->accept(cbv);
	osg::BoundingBox bb = cbv.getBoundingBox();
	for (int i = 0; i < 8; i++)
	{
		osg::Vec3 v = bb.corner(i);
		std::cout << v.x() << ", " << v.y() << ", " << v.z() << std::endl;
	}

	float z = -10.0f;
	osg::ref_ptr<osg::MatrixTransform> reverse = new osg::MatrixTransform;
	reverse->postMult(osg::Matrix::translate(0.0f, 0.0f, -z) *
		osg::Matrix::scale(1.0f, 1.0f, -1.0f) *
		osg::Matrix::translate(0.0f, 0.0f, z));
	reverse->addChild(scene.get());

	osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
	clipPlane->setClipPlane(0.0, 0.0, -1.0, z - 7.0);
	clipPlane->setClipPlaneNum(0);

	osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode;
	clipNode->addClipPlane(clipPlane.get());
	clipNode->addChild(reverse.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(scene.get());
	root->addChild(refNode.get());
	// root->addChild(reverse.get());
	root->addChild(clipNode.get());

	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());
	return viewer.run();
}