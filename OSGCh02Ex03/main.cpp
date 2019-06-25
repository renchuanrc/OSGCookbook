#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/ComputeBoundsVisitor>
#include <osg/AnimationPath>
#include <osg/PolygonMode>

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

// 围绕盒随物体运动而变化.
// osg::computeLocalToWorld 计算转换到世界的矩阵. osg::computeWorldToLocal() 相反的函数.
// osg::ComputeBoundVisitor 类计算每个节点的围绕盒都是在其父节点的坐标系内进行计算.
// 使用 localToWorld 矩阵将其转换至世界空间坐标.
class BoundingboxCallback : public osg::NodeCallback
{
public:
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::BoundingBox bb;
		for (unsigned int i = 0; i < _nodesToCompute.size(); i++)
		{
			osg::Node* node = _nodesToCompute[i];
			osg::ComputeBoundsVisitor cbbv;
			node->accept(cbbv);

			osg::BoundingBox localBB = cbbv.getBoundingBox();
			osg::Matrix localToWorld = osg::computeLocalToWorld(
				node->getParent(0)->getParentalNodePaths()[0]
			);
			for (unsigned int j = 0; j < 8; j++)
			{
				bb.expandBy(localBB.corner(j)*localToWorld);
			}
		}

		osg::MatrixTransform* trans = static_cast<osg::MatrixTransform*>(node);
		if (trans)
		{
			trans->setMatrix(
				osg::Matrix::scale(bb.xMax() - bb.xMin(), bb.yMax() - bb.yMin(), bb.zMax() - bb.zMin())*
				osg::Matrix::translate(bb.center())
			);
		}
	}

	// 保存所有需要计算的节点.
	osg::NodePath _nodesToCompute;
};

osg::AnimationPath* createAnimationPath(float radius, float time)
{
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
	path->setLoopMode(osg::AnimationPath::LOOP);

	unsigned int numSamples = 32;
	float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
	float delta_time = time / (float)numSamples;
	for (unsigned int i = 0; i < numSamples; i++)
	{
		float yaw = delta_yaw * (float)i;
		osg::Vec3 pos(sinf(yaw)*radius, cosf(yaw)*radius, 0.0f);
		osg::Quat rot(-yaw, osg::Z_AXIS);
		// 路径插入所有的控制点.
		path->insert(delta_time*(float)i,
			osg::AnimationPath::ControlPoint(pos, rot));
	}
	return path.release();
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::MatrixTransform> cessna =
		new osg::MatrixTransform;
	cessna->addChild(
		osgDB::readNodeFile("cessna.osgt.0,0,90.rot")
	);

	osg::ref_ptr<osg::AnimationPathCallback> apcb =
		new osg::AnimationPathCallback;
	// 设置飞机的路径.
	apcb->setAnimationPath(createAnimationPath(50.0f, 6.0f));
	cessna->setUpdateCallback(apcb.get());

	osg::ref_ptr<osg::MatrixTransform> dumptruck =
		new osg::MatrixTransform;
	dumptruck->addChild(osgDB::readNodeFile("dumptruck.osgt"));
	dumptruck->setMatrix(osg::Matrix::translate(0.0f, 0.0f, -100.0f));

	osg::ref_ptr<osg::MatrixTransform> models =
		new osg::MatrixTransform;
	models->addChild(cessna.get());
	models->addChild(dumptruck.get());
	models->setMatrix(osg::Matrix::translate(0.0f, 0.0f, 200.0f));

	osg::ref_ptr<BoundingboxCallback> bbcb =
		new BoundingboxCallback;
	bbcb->_nodesToCompute.push_back(cessna.get());
	bbcb->_nodesToCompute.push_back(dumptruck.get());


	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(new osg::Box));

	osg::ref_ptr<osg::MatrixTransform> boundingBoxNode =
		new osg::MatrixTransform;
	boundingBoxNode->addChild(geode.get());
	boundingBoxNode->setUpdateCallback(bbcb.get());
	boundingBoxNode->getOrCreateStateSet()->setAttributeAndModes(
		new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,
			osg::PolygonMode::LINE)
	);
	boundingBoxNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(models.get());
	root->addChild(osgDB::readNodeFile("lz.osgt"));
	root->addChild(boundingBoxNode.get());

	osgViewer::Viewer viewer;
	//viewer.addEventHandler(new RemoveShapeHandler);
	viewer.setSceneData(root.get());
	return viewer.run();
}