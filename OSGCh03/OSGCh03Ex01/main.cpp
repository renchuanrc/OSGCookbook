#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/LineWidth>

#include <osgUtil/Optimizer>
#include <osgUtil/Tessellator>

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


int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(8);
	(*vertices)[0].set(0.0f, 0.0f, 0.0f);
	(*vertices)[1].set(3.0f, 0.0f, 0.0f);
	(*vertices)[2].set(3.0f, 0.0f, 3.0f);
	(*vertices)[3].set(0.0f, 0.0f, 3.0f);
	(*vertices)[4].set(1.0f, 0.0f, 1.0f);
	(*vertices)[5].set(2.0f, 0.0f, 1.0f);
	(*vertices)[6].set(2.0f, 0.0f, 2.0f);
	(*vertices)[7].set(1.0f, 0.0f, 2.0f);

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
	(*normals)[0].set(0.0f, -1.0f, 0.0f);

	osg::ref_ptr<osg::Geometry> polygon = new osg::Geometry;
	polygon->setVertexArray(vertices.get());
	polygon->setNormalArray(normals.get());
	polygon->setNormalBinding(osg::Geometry::BIND_OVERALL);
	polygon->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
	polygon->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 4, 4));

	osgUtil::Tessellator tessellator;
	tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
	tessellator.retessellatePolygons(*polygon);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 0.0, 1.0f);

	osg::ref_ptr<osg::Geometry> border = new osg::Geometry;
	border->setVertexArray(vertices.get());
	border->setColorArray(colors.get());
	border->setColorBinding(osg::Geometry::BIND_OVERALL);
	border->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, 4));
	border->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 4, 4));
	border->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.0f));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(polygon.get());
	geode->addDrawable(border.get());

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setSceneData(geode.get());
	return viewer.run();
}
