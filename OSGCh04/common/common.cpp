#include "Common.h"
#include <osgText/Font>
#include <osgText/Text>
#include <osg/AnimationPath>

namespace osgCookBook
{
	osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");
	
	osg::Camera* createHUDCamera(double left, double right, double bottom, double top)
	{
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
		// 参考帧为绝对坐标系
		camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		// 渲染至纹理设置 PRE_RENDER, HUD 则用 POST_RENDER
		camera->setRenderOrder(osg::Camera::POST_RENDER);
		// 该相机关联的窗口是否允许生成事件
		camera->setAllowEventFocus(false);
		camera->setProjectionMatrix(
			osg::Matrix::ortho2D(left, right, bottom, top));
		// 禁止光照
		camera->getOrCreateStateSet()->setMode(
			GL_LIGHTING, osg::StateAttribute::OFF);
	
		// 减少计数, 由其他部分控制该指针
		return camera.release();
	}
	
	osgText::Text* createText(const osg::Vec3& pos, const std::string& content, float size)
	{
		osg::ref_ptr<osgText::Text> text = new osgText::Text;
		text->setDataVariance(osg::Object::DYNAMIC);
		text->setFont(g_font.get());
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::TextBase::XY_PLANE);
		text->setPosition(pos);
		text->setText(content);
		return text.release();
	}

	osg::AnimationPathCallback* createAnimationPathCallback(float radius, float time)
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
			path->insert(delta_time*(float)i, osg::AnimationPath::ControlPoint(pos, rot));
		}

		osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
		apcb->setAnimationPath(path.get());
		return apcb.release();
	}
}
