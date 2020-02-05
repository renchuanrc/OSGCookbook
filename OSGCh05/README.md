* OSG 典型的动画类型有路径动画, 纹理动画, 变形动画, 状态动画, 粒子动画, 骨骼动画.
* osgAnimation 库提供了一个完整的框架用于处理不同类型的动画.
* osgParticle 库提供框架设计和渲染粒子

# OSGCh05Ex01
* 门和入口在一些空间索引算法中很重要, 例如 PVS(潜在可见集-potentially visible set).
* 动画系统中的重要内容
    - 管理器回调(osgAnimation::BasicAnimatioinManager 或者其他) 管理动画数据, 其包括不同类型的 channels 和 keyframes. 其必须设置在根节点上, 这样才能处理所有子节点的动画.
    - updaters(osgAnimation::UpdateMatrixTransform及其它)必须设置为动画节点的回调. updaters 可以与一个channel 或多个 channel 连接作为动画目标. 它的 stacked 元素, 表示不同的动画关键类型, 必须与 channel 相匹配.
    - osg 的 channel 可看成关键帧数据和插值器的轨迹. stacked elements 看成关键过滤器, 定义可动的关键类型(位置, 旋转等等)
