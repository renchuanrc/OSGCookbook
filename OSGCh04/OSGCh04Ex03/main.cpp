#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>

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

const double radius_earth = 6378.137;
const double radius_sun = 695990.0;
const double AU = 149697900.0;

osg::Node* createScene()
{
	osg::ref_ptr<osg::ShapeDrawable> earth_sphere = new osg::ShapeDrawable;
	earth_sphere->setShape(new osg::Sphere(osg::Vec3(), radius_earth));
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile("Images/land_shallow_topo_2048.jpg"));
	osg::ref_ptr<osg::Geode> earth_node = new osg::Geode;
	earth_node->addDrawable(earth_sphere.get());
	earth_node->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

	osg::ref_ptr<osg::ShapeDrawable> sun_sphere = new osg::ShapeDrawable;
	sun_sphere->setShape(new osg::Sphere(osg::Vec3(), radius_sun));
	sun_sphere->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	osg::ref_ptr<osg::Geode> sun_geode = new osg::Geode;
	sun_geode->addDrawable(sun_sphere.get());
	osg::ref_ptr<osg::MatrixTransform> sun_node = new osg::MatrixTransform;
	sun_node->setMatrix(osg::Matrix::translate(0.0, AU, 0.0));
	sun_node->addChild(sun_geode.get());

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(earth_node.get());
	root->addChild(sun_node.get());
	return root.release();
}

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc, argv);

	double zNear = 1.0, zMid = 1e4, zFar = 2e8;
	arguments.read("--depth-partition", zNear, zMid, zFar);

	osg::ref_ptr<osgViewer::DepthPartitionSettings> dps = new osgViewer::DepthPartitionSettings;
	// 使用固定的数字作为划分的值
	dps->_mode = osgViewer::DepthPartitionSettings::FIXED_RANGE;
	dps->_zNear = zNear;
	dps->_zMid = zMid;
	dps->_zFar = zFar;

	osgViewer::Viewer viewer;
	viewer.getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	viewer.setSceneData(createScene());
	viewer.setUpDepthPartition(dps.get());

	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.getCameraManipulator()->setHomePosition(
		osg::Vec3d(0.0, -12.5*radius_earth, 0.0), osg::Vec3d(), osg::Vec3d(0.0, 0.0, 1.0));
	viewer.home();
	// viewer.setSceneData(root.get());
	return viewer.run();
}
