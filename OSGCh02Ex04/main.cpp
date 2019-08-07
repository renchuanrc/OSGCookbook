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
	// The prototype of the main rod
	osg::ref_ptr<osg::ShapeDrawable> mainRodShape =
		new osg::ShapeDrawable(new osg::Cylinder(
			osg::Vec3(), 0.4f, 10.0f));
	// The prototype of the coupling (wheel) rod
	osg::ref_ptr<osg::ShapeDrawable> wheelRodShape =
		new osg::ShapeDrawable(new osg::Cylinder(
			osg::Vec3(), 0.4f, 8.0f));
	// The prototypes of the wheel and the car body
	osg::ref_ptr<osg::ShapeDrawable> wheelShape =
		new osg::ShapeDrawable(new osg::Cylinder(
			osg::Vec3(), 2.0f, 1.0f));

	osg::ref_ptr<osg::ShapeDrawable> bodyShape =
		new osg::ShapeDrawable(new osg::Box(
			osg::Vec3(), 6.0f, 4.0f, 14.0f));

	osg::MatrixTransform* wheel1 = createTransformNode(
		wheelShape.get(), osg::Matrix::translate(0.0f, 0.0f, -4.0f));
	wheel1->setUpdateCallback(
		createWheelAnimation(osg::Vec3(0.0f, 0.0f, -4.0f)));

	osg::MatrixTransform* wheel2 = createTransformNode(
		wheelShape.get(), osg::Matrix::translate(0.0f, 0.0f, 4.0f));
	wheel2->setUpdateCallback(
		createWheelAnimation(osg::Vec3(0.0f, 0.0f, 4.0f)));

	osg::MatrixTransform* wheelRod1 = createTransformNode(
		wheelRodShape.get(),
		osg::Matrix::rotate(osg::Z_AXIS, osg::X_AXIS) *
		osg::Matrix::translate(0.0f, 0.0f, -5.0f));
	wheelRod1->addChild(wheel1);
	wheelRod1->addChild(wheel2);

	osg::MatrixTransform* wheelRod2 =
		static_cast<osg::MatrixTransform*>(
			wheelRod1->clone(osg::CopyOp::SHALLOW_COPY));
	wheelRod2->setMatrix(osg::Matrix::rotate(osg::Z_AXIS,
		osg::X_AXIS) * osg::Matrix::translate(0.0f, 0.0f, 5.0f));

	osg::MatrixTransform* body = createTransformNode(
		bodyShape.get(), osg::Matrix::translate(0.0f, 2.2f, 0.0f));
	osg::MatrixTransform* mainRod = createTransformNode(
		mainRodShape.get(), osg::Matrix::identity());
	mainRod->addChild(wheelRod1);
	mainRod->addChild(wheelRod2);
	mainRod->addChild(body);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(mainRod);
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setSceneData(root.get());
	return viewer.run();
}