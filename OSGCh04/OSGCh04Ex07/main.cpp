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

class FollowUpdater : public osgGA::GUIEventHandler
{
public:
	FollowUpdater(osg::Node* node) : _target(node) {}
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	osg::Matrix computeTargetToWorldMatrix(osg::Node* node) const;
protected:
	osg::observer_ptr<osg::Node> _target;
};

bool FollowUpdater::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* view = static_cast<osgViewer::View*>(&aa);
	if (!view || !_target || ea.getEventType() != osgGA::GUIEventAdapter::FRAME) return false;
	osgGA::OrbitManipulator* orbit = dynamic_cast<osgGA::OrbitManipulator*>(view->getCameraManipulator());
	if (orbit)
	{
		osg::Matrix matrix = computeTargetToWorldMatrix(_target.get());
		osg::Vec3d targetCenter = _target->getBound().center() * matrix;
		orbit->setCenter(targetCenter);
	}
	return false;
}

osg::Matrix FollowUpdater::computeTargetToWorldMatrix(osg::Node* node) const
{
	osg::Matrix l2w;
	if (node && node->getNumParents() > 0)
	{
		osg::Group* parent = node->getParent(0);
		l2w = osg::computeLocalToWorld(parent->getParentalNodePaths()[0]);
	}
	return l2w;
}

int main(int argc, char** argv)
{
	osg::Node* model = osgDB::readNodeFile("cessna.osg.0,0,90.rot");
	if (!model) return 1;
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	trans->addUpdateCallback(osgCookBook::createAnimationPathCallback(100.0f, 20.0));
	trans->addChild(model);
	osg::ref_ptr<osg::MatrixTransform> terrain = new osg::MatrixTransform;
	terrain->addChild(osgDB::readNodeFile("lz.osg"));
	terrain->setMatrix(osg::Matrix::translate(0.0f, 0.0f, -200.0f));
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(trans.get());
	root->addChild(terrain.get());

	osgViewer::Viewer viewer;
	viewer.addEventHandler(new FollowUpdater(model));
	viewer.setSceneData(root.get());
	return viewer.run();
}
