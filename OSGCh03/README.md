# OSGCh03Ex01
* 分形化一个凹多边形
* osgUtil::Tessellator
	- setTessellationType
		- TESS_TYPE_GEOMETRY: 分形化的所有几何体被添加至图元集合(有三角形, 四边形, 多边形)
		- TESS_TYPE_POLYGON: 分形化后的几何体只有多边形被添加至图元集合.
	- 内部使用 OpenGL 的分形化算法实现
	- retessellatePolygons() 分形化几何体

# OSGCh03Ex02
* 从 OSG 的二维形状推挤出三维物体
* osgUtil::Tessellator: 分形化多边形
* osgUtil::SmoothingVisitor::smooth 生成法线
* 主要是朝一个方向添加对应的多边形顶点数组, 而后分形化, 最后添加边处的多边形.

# OSGCh03Ex03
* 从 OSG 生成NURBS 样条, 通过 osg::Drawable 派生
* 重实现 computeBoundingBox, drawImplementation, 在 drawImplementation 进行绘制工作.

# OSGCh03Ex04
* 绘制时钟
* osg::MatrixTransform 变换矩阵节点, 添加动画路径更新回调实现时钟的更新
* osg::AnimationPath 需要设置 LoopMode, 插入控制点

# OSGCh03Ex05
* 实现缎带效果
* 预先分配好顶点数组和法线数组颜色等
* 变换矩阵节点回调一个更新回调类
* 在该回调类中使用变换矩阵节点自带的矩阵更新顶点数组的最后两个顶点位置, 其他顶点则读取后面的两个顶点位置更新
* dirty 更新顶点数组和法线数组, dirtyBound 更新围绕盒
* VBO 用来实现动态数据, Geometry 调用 setDataVariance 设置动态类型

# OSGCh03Ex06 
* 高亮选中物体
* osgUtil::LineSegmentIntersector::Intersection::drawable 获得相应几何体
* 选中一个几何体改变其颜色
* 其他选中的样式: 围绕盒, 网格形式(osgscribe), 轮廓(osgoutline), osgFX::Cartoon

# OSGCh03Ex07
* 高亮选中的三角形
* 创建一个三角形几何体, 可以覆盖在选中的地方上高亮
* 关键是 osgUtil::LineSegmentIntersector::Intersection::indexList 会返回选中所有三角形的索引列表, 从最近到最远
* osg::computeLocalToWorld 根据 osgUtil::LineSegmentIntersector::Intersection::nodePath 计算到世界的矩阵
* osgViewer::Viewer::addEventHandler 添加事件处理
* 使用 osg::PolygonOffset 增加偏移
* 注意 selector 不要作为其他节点的子节点, 否则需要计算其世界到局部的矩阵
* osg::Node::getParentalNodePaths() 获取父节点的节点路径以便于可以计算世界到局部或者局部到世界的矩阵


# OSGCh03Ex08
* 高亮选中的一个点, 思路为计算所有可能点与相交点的距离, 然后高亮最近的点
* 高亮的点设置 Osg::Point 状态属性
* osgUtil::LineSegmentIntersector::Intersection::drawable 获得几何体
* osgUtil::LineSegmentIntersector::Intersection::.getWorldIntersectPoint --- 获得交点在世界坐标系下的坐标
* 获取相机的视觉矩阵以及投影矩阵, 世界坐标系中的点乘以该矩阵, 在单元设备坐标空间内进行比较
* osg::Camera::getViewMatrix, osg::Camera::getProjectionMatrix --- 获得相机下的视图矩阵和投影矩阵
* 禁止相机的 osg::CullSettings::SMALL_FEATURE_CULLING 模式, 该模式用于后端(后台)渲染时自动忽略只有一个顶点的几何体
* 使用投影空间则是因为避免相机缩放场景时, 当物体非常小的时候, 选中一个顶点则非常困难.
* 该例的选择有个缺陷, 就是用户必须点中模型, 才有可能选择其中某个顶点, 如果没有点中模型, 就算距离模型某个顶点非常近, 也无法选中该点

# OSGCh03Ex09
* Vertex displacement mapping 在顶点着色器中使用纹理修改顶点位置
* 顶点着色器使用纹理坐标读取纹理值, 修改顶点位置
* 创建几何体注意调用 setInitialBound 设置初始围绕盒, 这是由于你会在着色器中修改顶点位置, 而OSG自己计算的围绕盒则有可能不一致, 因此OSG生成的相机投影矩阵的近截面和远截面的距离可能与你所需要的不一样.
* osg::Program 可以添加着色器 addShader
* 根据图案生成地形, 非常漂亮
* osgFX 有个类似的实现, osgfxbrowser

# OSGCh03Ex10
* 几何体实例化技术, 着色器处理每个实例
* osg::DrawArrays 或 osg::DrawElements 类都有一个 numInstances 参数表示实例化的对象. 注意要自己设置一个围绕盒.
* 本例设置多个实例, 每个实例不同的位置和颜色
* 还可见例子 osgdrawinstanced