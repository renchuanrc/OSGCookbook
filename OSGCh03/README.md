# OSGCh03Ex01
* 分形化一个凹多边形
* osgUtil::Tessellator
	- setTessellationType
		- TESS_TYPE_GEOMETRY: 分形化的所有几何体被添加至图元集合(有三角形, 四边形, 多边形)
		- TESS_TYPE_POLYGON: 分形化后的几何体只有多边形被添加至图元集合.
	- 内部使用 OpenGL 的分形化算法实现

# OSGCh03Ex02
* 从 OSG 的二维形状推挤出三维物体
* osgUtil::Tessellator: 分形化多边形
* osgUtil::SmoothingVisitor::smooth 生成法线
