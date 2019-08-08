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

class RemoveShapeHandler : public osgCookBook::PickHandler
{
	virtual void doUserOperations(
		const osgUtil::LineSegmentIntersector::Intersection& result)
	{
		if (result.nodePath.size() > 0)
		{
			osg::Geode* geode = dynamic_cast<osg::Geode*>(
				result.nodePath.back());
			if (geode)
			{
				geode->removeDrawable(result.drawable.get());
			}
		}
	}
};

class ObserverShapeCallback : public osg::NodeCallback
{
public:
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		std::string content;
		if (_drawable1.valid()) content += "Drawable1; ";
		if (_drawable2.valid()) content += "Drawable2; ";
		if (_text.valid()) _text->setText(content);
	}

	osg::ref_ptr<osgText::Text> _text;
	osg::observer_ptr<osg::Drawable> _drawable1;
	osg::observer_ptr<osg::Drawable> _drawable2;
};

int main(int argc, char** argv)
{
	osgText::Text* text = osgCookBook::createText(osg::Vec3(
		50.0f, 50.0f, 0.0f), "", 10.0f);
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable(text);

	osg::ref_ptr<osg::Camera> hudCamera =
		osgCookBook::createHUDCamera(0, 800, 0, 600);
	hudCamera->addChild(textGeode);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Box(osg::Vec3(-2.0f, 0.0f, 0.0f), 1.0f)));
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(2.0f, 0.0f, 0.0f), 1.0f)));

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(hudCamera.get());
	root->addChild(geode.get());

	osg::ref_ptr<ObserverShapeCallback> observerCB = new ObserverShapeCallback;
	observerCB->_text = text;
	observerCB->_drawable1 = geode->getDrawable(0);
	observerCB->_drawable2 = geode->getDrawable(1);
	root->addUpdateCallback(observerCB.get());

	osgViewer::Viewer viewer;
	viewer.addEventHandler(new RemoveShapeHandler);
	viewer.setSceneData(root.get());
	return viewer.run();
}