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

void convertLongLatHeightToXYZ(osg::Vec3d& out_vecXYZ, double in_lon, double in_lat, double in_hei)
{
	osg::EllipsoidModel em;
	double x, y, z;
	em.convertLatLongHeightToXYZ(in_lat, in_lon, in_hei, x, y, z);
	out_vecXYZ = osg::Vec3d(x, y, z);
}

class BillboardCallback : public osg::NodeCallback
{
public:
	BillboardCallback(osg::MatrixTransform* billboard)
		: _billboardNode(billboard)
	{}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		// 确保是在剔除回调中进行
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
		if (_billboardNode.valid() && cv)
		{
			osg::Vec3d translation, scale;
			osg::Quat rotation, so; // so -> scale orientation

			cv->getModelViewMatrix()->decompose(translation, rotation, scale, so);
			osg::Matrixd matrix(rotation.inverse());
			_billboardNode->setMatrix(matrix);
		}

		traverse(node, nv);
	}

protected:
	osg::observer_ptr<osg::MatrixTransform> _billboardNode;
};

void caculateQuatToNorth(osg::Quat& out_quat, const osg::Vec3d& in_pos)
{
	// 将pos由XYZ转为经纬高
	osg::Vec3d vecLLH = in_pos;

	// 先绕Z轴旋转经度
	osg::Quat quatZ(vecLLH.x(), osg::Z_AXIS);
	osg::Vec3d X_AXIS_after_quatZ; // 绕Z轴旋转之后的X轴方向
	osg::Vec3d Y_AXIS_after_quatZ; // 绕Z轴旋转之后的Y轴方向

	X_AXIS_after_quatZ = quatZ * osg::X_AXIS;
	Y_AXIS_after_quatZ = quatZ * osg::Y_AXIS;

	// 再绕当前的Y轴反向旋转纬度
	osg::Quat quatY(-vecLLH.y(), Y_AXIS_after_quatZ);
	osg::Vec3d X_AXIS_after_quatY; // 绕Y轴旋转之后的X轴方向
	osg::Vec3d Z_AXIS_after_quatY; // 绕Y轴旋转之后的Z轴方向

	X_AXIS_after_quatY = quatY * X_AXIS_after_quatZ;
	Z_AXIS_after_quatY = quatY * osg::Z_AXIS;

	// 此时：
	//		X_AXIS_after_quatY指向天
	//		Y_AXIS_after_quatZ指向东
	//		Z_AXIS_after_quatY指向北

	// 现在要让Y轴正北向，因此，先绕当前Z轴旋转90度，再绕当前Y轴旋转90度
	out_quat = quatZ * quatY * osg::Quat(osg::PI_2, Z_AXIS_after_quatY) * osg::Quat(osg::PI_2, Y_AXIS_after_quatZ);
}

int main(int argc, char** argv)
{
	/*
	osg::Vec3d worldPos;
	convertLongLatHeightToXYZ(worldPos, 0.0, osg::PI_4, 0.0);
	std::cout << worldPos.x() << ", " << worldPos.y() << ", " << worldPos.z() << std::endl;

	osg::Matrix m = osg::Matrix::rotate(osg::PI_4, osg::Vec3(0.0, 0.0, 1.0));

	osg::ref_ptr<osg::MatrixTransform> billboardNode = new osg::MatrixTransform;
	billboardNode->addChild(osgDB::readNodeFile("cessna.osg"));

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(billboardNode.get());
	root->addChild(osgDB::readNodeFile("lz.osg"));
	root->addCullCallback(
		new BillboardCallback(billboardNode.get()));
	*/
	// 0.70710678118654746
	osg::Matrix m = osg::Matrix::rotate(osg::PI_4, osg::Vec3(0.0, 0.0, 1.0));
	osg::Quat q(osg::PI_2, osg::Z_AXIS);
	caculateQuatToNorth(q, osg::Vec3d(0.0, 0.0, 0.0));
	q.get(m);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	osg::Matrix mat = osg::Matrix::rotate(osg::PI_4, osg::X_AXIS)*osg::Matrix::translate(0.0, 0.0, -30.0);
	root->addChild(osgDB::readNodeFile("cow.osg"));
	mt->setMatrix(mat);
	mt->addChild(osgDB::readNodeFile("cessna.osg"));
	root->addChild(mt.get());

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 800, 600);
	viewer.setSceneData(root.get());
	return viewer.run();
}
