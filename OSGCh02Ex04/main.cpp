#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/AnimationPath>

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

// 矩阵变换节点使用矩阵参数和geode, 并添加形状.
osg::MatrixTransform* createTransformNode(osg::Drawable* shape, const osg::Matrix& matrix)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(shape);
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->addChild(geode.get());
	trans->setMatrix(matrix);
	return trans.release();
}

osg::AnimationPathCallback* createWheelAnimation(const osg::Vec3& base)
{
	osg::ref_ptr<osg::AnimationPath> wheelPath =
		new osg::AnimationPath;
	wheelPath->setLoopMode(osg::AnimationPath::LOOP);
	wheelPath->insert(0.0, osg::AnimationPath::ControlPoint(base, osg::Quat()));
	wheelPath->insert(0.01, osg::AnimationPath::ControlPoint(base + osg::Vec3(0.0f, 0.02f, 0.0f), osg::Quat(osg::PI_2, osg::Z_AXIS)));
	wheelPath->insert(0.02, osg::AnimationPath::ControlPoint(base + osg::Vec3(0.0f, -0.02f, 0.0f), osg::Quat(osg::PI, osg::Z_AXIS)));
	osg::ref_ptr<osg::AnimationPathCallback> apcb =
		new osg::AnimationPathCallback;
	apcb->setAnimationPath(wheelPath.get());
	return apcb.release();
}

int main(int argc, char** argv)
{
	osgViewer::Viewer viewer;
	// viewer.setSceneData(root.get());
	return viewer.run();
}