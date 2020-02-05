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

#include <osg/GL>
#include <gl/GLU.h>

#pragma comment(lib, "opengl32.lib")

class NurbsSurface : public osg::Drawable
{
public:
	NurbsSurface() : _sCount(0), _tCount(0), _sOrder(0), _tOrder(0), _nurbsObj(0) {}
	NurbsSurface(const NurbsSurface& copy, osg::CopyOp copyop = osg::CopyOp::SHALLOW_COPY);
	META_Object(osg, NurbsSurface);

	void setVertexArray(osg::Vec3Array* va) { _vertices = va; }
	void setNormalArray(osg::Vec3Array* na) { _normals = na; }
	void setTexCoordArray(osg::Vec2Array* ta) { _texcoords = ta; }
	void setKnots(osg::FloatArray* sknots, osg::FloatArray* tknots)
	{
		_sKnots = sknots; _tKnots = tknots;
	}
	void setCounts(int s, int t) { _sCount = s; _tCount = t; }
	void setOrders(int s, int t) { _sOrder = s; _tOrder = t; }

	virtual osg::BoundingBox computeBoundingBox() const;
	virtual void drawImplementation(osg::RenderInfo& renderInfo) const;

protected:
	virtual ~NurbsSurface() {}

	osg::ref_ptr<osg::Vec3Array> _vertices;
	osg::ref_ptr<osg::Vec3Array> _normals;
	osg::ref_ptr<osg::Vec2Array> _texcoords;
	osg::ref_ptr<osg::FloatArray> _sKnots;
	osg::ref_ptr<osg::FloatArray> _tKnots;
	int _sCount, _tCount;
	int _sOrder, _tOrder;
	mutable void* _nurbsObj;
};

NurbsSurface::NurbsSurface(const NurbsSurface& copy, osg::CopyOp copyop)
	: osg::Drawable(copy, copyop), _vertices(copy._vertices),
	_normals(copy._normals), _texcoords(copy._texcoords),
	_sKnots(copy._sKnots), _tKnots(copy._tKnots),
	_sOrder(copy._sOrder), _tOrder(copy._tOrder),
	_nurbsObj(copy._nurbsObj)
{

}

osg::BoundingBox NurbsSurface::computeBoundingBox() const
{
	osg::BoundingBox bb;
	if (_vertices.valid())
	{
		for (unsigned int i = 0; i < _vertices->size(); ++i)
			bb.expandBy((*_vertices)[i]);
	}
	return bb;
}

void NurbsSurface::drawImplementation(osg::RenderInfo&
	renderInfo) const
{
	GLUnurbsObj* theNurbs = (GLUnurbsObj*)_nurbsObj;
	if (!theNurbs)
	{
		theNurbs = gluNewNurbsRenderer();
		gluNurbsProperty(theNurbs, GLU_SAMPLING_TOLERANCE, 10);
		gluNurbsProperty(theNurbs, GLU_DISPLAY_MODE, GLU_FILL);
		_nurbsObj = theNurbs;
	}

	if (_vertices.valid() && _sKnots.valid() && _tKnots.valid())
	{
		glEnable(GL_MAP2_NORMAL);
		glEnable(GL_MAP2_TEXTURE_COORD_2);
		gluBeginCurve(theNurbs);
		if (_texcoords.valid())
		{
			gluNurbsSurface(theNurbs, _sKnots->size(),
				&((*_sKnots)[0]), _tKnots->size(), &((*_tKnots)[0]),
				_sCount * 2, 2, &((*_texcoords)[0][0]), _sOrder, _tOrder,
				GL_MAP2_TEXTURE_COORD_2);
		}
		if (_normals.valid())
		{
			gluNurbsSurface(theNurbs, _sKnots->size(),
				&((*_sKnots)[0]), _tKnots->size(), &((*_tKnots)[0]),
				_sCount * 3, 3, &((*_normals)[0][0]), _sOrder, _tOrder,
				GL_MAP2_NORMAL);
		}
		gluNurbsSurface(theNurbs, _sKnots->size(),
			&((*_sKnots)[0]), _tKnots->size(), &((*_tKnots)[0]),
			_sCount * 3, 3, &((*_vertices)[0][0]), _sOrder, _tOrder,
			GL_MAP2_VERTEX_3);
		gluEndCurve(theNurbs);
		glDisable(GL_MAP2_NORMAL);
		glDisable(GL_MAP2_TEXTURE_COORD_2);
	}
}


int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Vec3Array> ctrlPoints = new osg::Vec3Array;
#define ADD_POINT(x, y, z) ctrlPoints->push_back(osg::Vec3(x, y, z));
	ADD_POINT(-3.0f, 0.5f, 0.0f); ADD_POINT(-1.0f, 1.5f, 0.0f);
	ADD_POINT(-2.0f, 2.0f, 0.0f);
	ADD_POINT(-3.0f, 0.5f, -1.0f); ADD_POINT(-1.0f, 1.5f, -1.0f);
	ADD_POINT(-2.0f, 2.0f, -1.0f);
	ADD_POINT(-3.0f, 0.5f, -2.0f); ADD_POINT(-1.0f, 1.5f, -2.0f);
	ADD_POINT(-2.0f, 2.0f, -2.0f);
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

#define ADD_TEXCOORD(x, y) texcoords->push_back(osg::Vec2(x, y));
	ADD_TEXCOORD(0.0f, 0.0f); ADD_TEXCOORD(0.5f, 0.0f);
	ADD_TEXCOORD(1.0f, 0.0f);
	ADD_TEXCOORD(0.0f, 0.5f); ADD_TEXCOORD(0.5f, 0.5f);
	ADD_TEXCOORD(1.0f, 0.5f);
	ADD_TEXCOORD(0.0f, 1.0f); ADD_TEXCOORD(0.5f, 1.0f);
	ADD_TEXCOORD(1.0f, 1.0f);
	osg::ref_ptr<osg::FloatArray> knots = new osg::FloatArray;
	knots->push_back(0.0f); knots->push_back(0.0f);
	knots->push_back(0.0f);
	knots->push_back(1.0f); knots->push_back(1.0f);
	knots->push_back(1.0f);

	osg::ref_ptr<NurbsSurface> nurbs = new NurbsSurface;
	nurbs->setVertexArray(ctrlPoints.get());
	nurbs->setTexCoordArray(texcoords.get());
	nurbs->setKnots(knots.get(), knots.get());
	nurbs->setCounts(3, 3);
	nurbs->setOrders(3, 3);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(nurbs.get());
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new osg::Texture2D(osgDB::readImageFile(
			"Images/osg256.png")));
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,
		osg::StateAttribute::OFF);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(geode.get());

	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());
	return viewer.run();
}
