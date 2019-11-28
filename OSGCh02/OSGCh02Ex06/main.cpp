#include <iostream>
#include <osg/Notify>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/NodeVisitor>
#include <osgUtil/PrintVisitor>
#include <deque>

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

class BFSVisitor : public osg::NodeVisitor
{
public:
	BFSVisitor() { setTraversalMode(TRAVERSE_ALL_CHILDREN); }
	
	// 重置所有成员变量为初始状态.
	virtual void reset() { _pendingNodes.clear(); }
	virtual void apply(osg::Node& node) { traverseBFS(node); }
protected:
	virtual ~BFSVisitor() {}
	void traverseBFS(osg::Node& node);

	std::deque<osg::Node*> _pendingNodes;
};

class BFSPrintVisitor : public BFSVisitor
{
public:
	virtual void apply(osg::Node& node);
};

void BFSPrintVisitor::apply(osg::Node& node)
{
	// 处理部分
	std::cout << node.libraryName() << "::" << node.className() << std::endl;
	traverseBFS(node);
}

void BFSVisitor::traverseBFS(osg::Node& node)
{
	osg::Group* group = node.asGroup();
	if (!group)
		return;

	for (unsigned int i = 0; i < group->getNumChildren(); ++i)
	{
		_pendingNodes.push_back(group->getChild(i));
	}

	while (_pendingNodes.size() > 0)
	{
		osg::Node* node = _pendingNodes.front();
		_pendingNodes.pop_front();
		node->accept(*this);
	}
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("osgcool.osgt");
	std::cout << "DFS Visitor traversal: " << std::endl;
	osgUtil::PrintVisitor pv(std::cout);
	root->accept(pv);
	std::cout << std::endl;

	std::cout << "BFS Visitor traversal:" << std::endl;
	BFSPrintVisitor bpv;
	root->accept(bpv);

	// osgViewer::Viewer viewer;
	// viewer.setSceneData(root.get());
	system("pause");
	return 0;
}
