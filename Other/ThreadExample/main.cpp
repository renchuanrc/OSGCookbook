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
#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <OpenThreads/Condition>
#include <OpenThreads/ScopedLock>
#include <OpenThreads/Block>

class TestThread : public OpenThreads::Thread
{
public:
	TestThread() : _done(false), _count(0) { }
	~TestThread() { cancel(); }

	void block() { _block.block(); }

	virtual int cancel()
	{
		_block.release();
		_done = true;
		while (isRunning())
		{
			OpenThreads::Thread::YieldCurrentThread();
		}
		return 0;
	}
	virtual void run()
	{
		do 
		{
			std::cout << "(" << _count << ")";
			++_count;

			if (_count == 10)
			{
				// 释放阻塞的线程, 可能是其他线程的阻塞
				_block.release();
				_block.reset();
				_block.block();
			}
			microSleep(150000);
		} while (!_done);
	}

protected:
	bool _done;
	unsigned long _count;
	OpenThreads::Block _block;
};


int main(int argc, char** argv)
{
	TestThread t;
	t.start();
	// 把主线程阻塞了
	t.block();
	std::cout << "(Main)" << std::endl;
	getchar();
	t.cancel();

	std::cout << std::endl;
	system("pause");
	return 0;
}
