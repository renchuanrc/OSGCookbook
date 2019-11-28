#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>

#include <osgUtil/Optimizer>
#include <osgUtil/CullVisitor>

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
#include <osg/CoordinateSystemNode>

class BillboardCallback : public osg::NodeCallback
{
public:
	BillboardCallback(osg::MatrixTransform* billboard)
		: _billboardNode(billboard) {}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (_billboardNode.valid() && cv)
		{
			osg::Vec3d translation, scale;
			osg::Quat rotation, so;
			cv->getModelViewMatrix()->decompose(translation, rotation, scale, so);

			osg::Matrixd matrix(rotation.inverse());
			_billboardNode->setMatrix(matrix);
		}
		traverse(node, nv);
	}

protected:
	osg::observer_ptr<osg::MatrixTransform> _billboardNode;
};

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::MatrixTransform> billboardNode = new osg::MatrixTransform;
	billboardNode->addChild(osgDB::readNodeFile("cessna.osg"));

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(billboardNode.get());
	root->addChild(osgDB::readNodeFile("lz.osg"));
	root->addCullCallback(new BillboardCallback(billboardNode.get()));

	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());
	return viewer.run();
}
