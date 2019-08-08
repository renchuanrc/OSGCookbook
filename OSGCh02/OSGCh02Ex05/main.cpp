#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/ClipNode>
#include <osg/MatrixTransform>

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
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile("cessna.osg");

	float z = -10.0f;
	osg::ref_ptr<osg::MatrixTransform> reverse = new osg::MatrixTransform;
	reverse->preMult(osg::Matrix::translate(0.0f, 0.0f, -z) *
		osg::Matrix::scale(1.0f, 1.0f, -1.0f) *
		osg::Matrix::translate(0.0f, 0.0f, z));
	reverse->addChild(scene.get());

	osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
	clipPlane->setClipPlane(0.0, 0.0, -1.0, z);
	clipPlane->setClipPlaneNum(0);

	osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode;
	clipNode->addClipPlane(clipPlane.get());
	clipNode->addChild(reverse.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(scene.get());
	root->addChild(clipNode.get());

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setSceneData(root.get());

	return viewer.run();
}
