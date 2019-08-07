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

int main(int argc, char** argv)
{
	osgViewer::Viewer viewer;
	// viewer.setSceneData(root.get());
	return viewer.run();
}
