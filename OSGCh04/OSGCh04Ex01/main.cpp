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
#include <osgViewer/CompositeViewer>
#include <iostream>
#include "Common.h"
#include "PickHandler.h"

osgViewer::View* createView(int screenNum)
{
	unsigned int width = 800, height = 600;
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if (wsi)
	{
		wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(screenNum), width, height);
	}

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new
		osg::GraphicsContext::Traits;
	traits->screenNum = screenNum;
	traits->x = 0;
	traits->y = 0;
	traits->width = width;
	traits->height = height;
	traits->windowDecoration = false; // ²»Òª±ß¿ò
	traits->doubleBuffer = true; // Ë«»º´æ
	traits->sharedContext = 0;

	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	if (!gc) return NULL;

	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(gc.get());
	camera->setViewport(new osg::Viewport(0, 0, width, height));
	camera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(width) / static_cast<double>(height),
		1.0f, 10000.0f);
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);

	osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
	view->setCamera(camera.get());
	view->setCameraManipulator(new osgGA::TrackballManipulator);
	return view.release();
}

int main(int argc, char** argv)
{
	osgViewer::CompositeViewer viewer;
	osgViewer::View* view1 = createView(0);
	if (view1)
	{
		view1->setSceneData(osgDB::readNodeFile("cessna.osg"));
		viewer.addView(view1);
	}

	osgViewer::View* view2 = createView(1);
	if (view2)
	{
		view2->setSceneData(osgDB::readNodeFile("cow.osg"));
		viewer.addView(view2);
	}

	// viewer.setSceneData(root.get());
	return viewer.run();
}
