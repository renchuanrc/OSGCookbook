# OSGCh04Ex01
* 在多个屏幕上显示视图
* 开发多屏幕程序的困难在于开发者需要自己处理每个屏幕的图形缓存.
* OSG 封装了具体平台的多显示器的APIs, 可以使用 context ID 区别屏幕
* WindowingSystemInterface 类可以得到具体Windows的APIs. 
* 注意设置 Traits 对象的 screenNum 值, 这是多显示器多视图的关键
* osg::GraphicsContext::createGraphicsContext 创建图形上下文, 如 traits 数据无效或者屏幕索引无效则返回 NULL
* 设置相机的图形上下文 osg::Camera::setGraphicsContext()
* 设置相机的视口 osg::Camera::setViewport()
* 设置相机的投影矩阵 osg::Camera::setProjectionMatrixAsPerspective
* 设置相机的读写 Buffer osg::Camera::setDrawBuffer()/setReadBuffer(), 双缓存则都是用 GL_BACK, 但缓存则使用 GL_FRONT
* 如果自己创建一个视图, 则设置它的相机和相机操作器, 如 osgGA::TrackballManipulator
* WindowingSystemInterface 类的 getNumScreens() 方法可以得到当前计算机所连接屏幕的数量.
    - getScreenSettings 可以得到特定屏幕更详细的信息, settings 变量可以得到屏幕宽度, 高度, 刷新率, 颜色 buffer 深度属性.
* 见例子 osgcamera

# OSGCh04Ex02
* 使用多个从属相机模拟威力墙, (多个显示器或者投影设备共享相同的数据组成更大的屏幕)
* 从属相机可以多个显示器, 也可以单个显示器
* 根据 traits 创建 graphics context, 创建相机连接至 graphics context. 设置相机的读写 buffer.
* 从属相机不设置自己的视图和投影矩阵, 其使用主相机的视图, 并各自矩阵里添加偏差.
* osgViewer::Viewer::addSlave 添加从属相机
* 从属相机读取主相机的视图和投影矩阵, 并乘以偏差矩阵
```
slaveViewMatrix = mainViewMatrix * viewOffset;
slaveProjectionMatrix = mainProjectionMatrix * projectionOffset;
```
* 偏差矩阵的计算可以通过 左右上下的投影矩阵变化来计算, 具体见 pdf 151 页
* 例子 osgcluster 可以处理多个相机的同步

# OSGCh04Ex03
* 显示一个庞大场景时的深度划分
* 当绘制太阳系这样类似场景时, 会产生类似 Z-fighting 的问题.
* depth partition 算法(深度划分), 其将场景划分成不同的部分, 每个部分各自渲染, 每个部分有自己的near和far值, 且由于这两者的距离足够近,所以可以避免精度的问题. 所以每一帧的场景可能会多次进行渲染.
* osg::ShapeDrawable 可以用于创建各种形状的几何体(Geometry), 
    - 调用 setShape 设置各种形状, 如 osg::Sphere.
    - 调用 setColor 可以设置该几何体的颜色
* osg::Matrix::translate 得到平移矩阵
* osgViewer::DepthPartitionSettings 可以创建一个深度划分, 设置其 mode, zNear, zMid, zFar 划分
* osgViewer::Viewer::setUpDepthPartition() 设置典型
* OSG 自动计算远近截平面. 基本思想如下: 计算视觉坐标系内每个场景的Z值, 而后得到最大的Z值, 将其作为最远的深度值. 而后该最远的深度值成员一个非常小的比例值, 得到最近的深度值.
```
    zNear = zFar * nearFarRatio
```
* depth partition 的缺点, 需要多次渲染场景. 
* 其他解决方法, 设置一个更小的比例值, setNearFarRatio(). 默认该值为 0.0005.
```
    camera->setNearFarRatio(0.0001);
```
* 注意你的 OpenGL 深度 buffer 可能只能处理 16 位或 24 位浮点数.

# OSGCh04Ex04
* 实现雷达图, 本例的雷达和相机位置没有关系, 只是从上方往下看的一个二维平面, 通过 CULL MASK 实现不同的相机显示相同的内容中的不同部分.
* osg::Node::setNodeMask 设置节点 Mask, 该 Mask 和相机有关.
* osg::Material 可以设置物体的材质, 有 setAmbient, setDiffuse, setSpecular, setShinness
* osg::Camera
    - setRenderOrder --- 设置其绘制的顺序
    - setAllowEventFocus --- 可以设置是否接收键盘鼠标和窗口事件
    - setClearMask --- 设置 Mask
    - setClearColor --- 设置背景颜色
    - setReferenceFrame --- 设置参考坐标系 osg::Transform::ABSOLUTE_RF 等
    - setViewport --- 设置视口
    - setViewMatrix
    - setProjectionMatrix
    - setCullMask -- 与 osg::Node 的 NodeMask 相互作用
* osg::Viewer 还可以设置光照模型(setLightingMode, osg::View::SKY_LIGHT), 其可以改进模型表面的光照计算. 该方法主要控制 OSG 内全局光照的机制. 默认值为 HEADLIGHT, 表示附近光源. 还可以设置 NO_LIGHT 无光源, SKY_LIGHT 天空光源. 还可以使用 setLight() 方法设置全局光照的具体光照.
* 立体渲染中, 还有 setCullMaskLeft() 和 setCullMaskRight(), 分别设置左右眼中的内容.

# OSGCh04Ex05
* 多个视图观察模型的不同面(顶面, 前面, 后面, 左面, 右面等)
* osgViewer::View 自带相机, 使用的时候设置其 GraphicsContext, Viewport, ViewMatrix, ProjectMatrix
* osg::Node::getBound() 得到围绕球, 可以获取其中心与半径.
* osg::Viewer 的 run() 函数自动为每个 View 应用一个 trackball 操作器, 除非 View 有自己的一个有效操作器
* 使用 osgViewer::CompositeViewer 显示多个视图
* 一个 OSG 场景 viewer 可以只有一个 View(osgViewer::Viewer), 也可以有多个 Views(osgViewer::CompositeViewer). 每个 View 有自己的场景图, 相机操作器, 事件处理函数. 一个 View 可以有一个主相机, 该主相机由操作器控制, 可以有多个从属相机, 其和主相机有自定义的偏差矩阵. 相机可以作为普通的节点添加至场景图中. 其会乘以或者重置当前的视图和投影矩阵, 使用当前的gc或者不同的gc渲染其对应的子图.

# OSGCh04Ex06
* 多个视图观察模型的不同面, 并与之互动
* osgGA::GUIEventHandler 派生类作为视图的事件处理
* osgGA::GUIActionAdapter 可以转化内osgGA::View, osgGA::GUIEventAdapter 可以获得事件类型, X/Y 坐标, osgGA::GUIEventAdapter::getWindowHeight() 还可以获得窗口高度.
* osgGA::GUIEventAdapter::FRAME 帧事件.
* osgViewer::View::addEventHandler 添加事件处理
* 通过修改相机的视图矩阵移动整个场景
* osg::Camera::getViewMatrixAsLookAt() 可以获得相机的参数, 然后根据 side 向量和 up 向量移动场景.

# OSGCh04Ex07
* 相机跟随物体运动
* 在帧事件处理中, 获取视图的相机操作器, 判断是否 osgGA::OrbitManipulator 类型
* 获取目标节点的世界坐标位置, 调用 osgGA::OrbitManipulator::setCenter 方法跟随目标
* osg::computeLocalToWorld 根据 nodePath 计算
    - osg::Group::getParentalNodePaths: 获取 nodePath
* 主要原理为帧事件中更新操作器的中心

# OSGCh04Ex08
* 使用 osgGA::NodeTrackerManipulator 类允许我们跟随一个静止和运动的物体. OSG 还提供 osgGA::FirstPersonManipulator 类用于行走, 驾驶, 飞行操作.
* osgGA::NodeTrackerManipulator
    - setHomePosition
    - setTrackerMode --- 跟踪模式
    - setRotationMode --- 旋转模式
    - setTrackNode --- 设置跟踪节点
* osgGA::KeySwitchMatrixManipulator 根据按键切换相机操作器
* osgGA::CameraManipulator 只能做用于视图的主相机. 每一帧中都会改变视图矩阵以实现相机的运动和浏览动作.
* 如果你想要实现画中画效果或者想要跟踪辅助相机中的节点, 最好使用回调更合适
* 事实上 osgGA::GUIEventHandler 类派生了相机的操作器. 但他们单独处理. 有额外的接口用于场景的浏览目的.

# OSGCh04Ex09
* 设计一个二维的相机操作器
* osgGA::CameraManipulator 类是所有相机操作器的抽象基类
* osgGA::StandardManipulator 类, 处理所有像鼠标点击和键盘按键的事件, 根据事件类型将事件内容发送给不同的虚方法.
    - getMatrix() 操作器的当前位置和姿势(attitude)矩阵
    - getInverseMatrix() 得到相机操作器的矩阵并求逆. 该你矩阵可以看成是相机的视图矩阵. 这两个方法是系统仅有的用于返回和应用视图矩阵的借口.
    - setByMatrix()/setByInverseMatrix() 可以被外面的代码所调用, 用于设置操作器的位置矩阵或者逆矩阵.