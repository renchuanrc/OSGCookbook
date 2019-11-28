#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

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


class Compass : public osg::Camera
{
public:
	Compass();
	Compass(const Compass& copy, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
	META_Node(osg, Compass);

	void setPlate(osg::MatrixTransform* plate) {
		_plateTransform = plate;
	}
	void setNeedle(osg::MatrixTransform* needle) {
		_needleTransform = needle;
	}
	void setMainCamera(osg::Camera* camera) {
		_mainCamera = camera;
	}

	virtual void traverse(osg::NodeVisitor& nv);

protected:
	virtual ~Compass();

	osg::ref_ptr<osg::MatrixTransform> _plateTransform;
	osg::ref_ptr<osg::MatrixTransform> _needleTransform;
	osg::observer_ptr<osg::Camera> _mainCamera;
};

Compass::Compass()
{
}

Compass::Compass(const Compass& copy, osg::CopyOp copyop)
	: osg::Camera(copy, copyop),
	_plateTransform(copy._plateTransform),
	_needleTransform(copy._needleTransform),
	_mainCamera(copy._mainCamera)
{
}

Compass::~Compass()
{
}

void Compass::traverse(osg::NodeVisitor& nv)
{
	if (_mainCamera.valid() && nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		osg::Matrix matrix = _mainCamera->getViewMatrix();
		matrix.setTrans(osg::Vec3());

		// 北极向量
		osg::Vec3 northVec = osg::Z_AXIS * matrix;
		northVec.z() = 0.0f;
		northVec.normalize();

		osg::Vec3 axis = osg::Y_AXIS ^ northVec;
		float angle = atan2(axis.length(), osg::Y_AXIS*northVec);
		axis.normalize();

		if (_plateTransform.valid())
		{
			_plateTransform->setMatrix(osg::Matrix::rotate(angle, axis));
		}
		_plateTransform->accept(nv);
		_needleTransform->accept(nv);
		osg::Camera::traverse(nv);
	}
}

osg::MatrixTransform* createCompassPart(const std::string& image,
	float radius, float height)
{
	osg::Vec3 center(-radius, -radius, height);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(createTexturedQuadGeometry(center, osg::Vec3(radius*2.0f, 0.0f, 0.0f),
		osg::Vec3(0.0f, radius*2.0f, 0.0f)));

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(image));

	osg::ref_ptr<osg::MatrixTransform> part = new osg::MatrixTransform;
	part->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
	part->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	part->addChild(geode.get());
	return part.release();
}

osg::Geode* createEarth(const std::string& filename)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(filename));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), osg::WGS_84_RADIUS_POLAR)));
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
	return geode.release();
}


int main(int argc, char** argv)
{
	osgViewer::Viewer viewer;

	osg::ref_ptr<Compass> compass = new Compass;
	compass->setMainCamera(viewer.getCamera());
	compass->setViewport(0, 0, 200, 200);
	compass->setProjectionMatrix(osg::Matrixd::ortho(
		-1.5, 1.5, -1.5, 1.5, -10.0, 10.0));
	compass->setPlate(createCompassPart("compass_plate.png", 1.5f, -1.0f));
	compass->setNeedle(createCompassPart("compass_needle.png", 1.5f, 0.0f));
	compass->setRenderOrder(osg::Camera::POST_RENDER);
	compass->setClearMask(GL_DEPTH_BUFFER_BIT);
	compass->setAllowEventFocus(false);
	compass->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	compass->getOrCreateStateSet()->setMode(GL_LIGHTING,
		osg::StateAttribute::OFF);
	compass->getOrCreateStateSet()->setMode(GL_BLEND,
		osg::StateAttribute::ON);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(createEarth("Images/land_shallow_topo_2048.jpg"));
	root->addChild(compass.get());
	viewer.setSceneData(root.get());
	viewer.setUpViewInWindow(50, 50, 800, 600);

	return viewer.run();
}
