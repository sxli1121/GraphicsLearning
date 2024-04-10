# GraphicsLearning

## 学习图形学的过程：包括三个阶段

### 1. 光栅化

- [x] 绘制直线(DDA 中心画线算法 Bresenham直线算法) 
- [x] 裁剪算法(中点裁剪  科恩·萨瑟兰德裁剪算法)  
- [x] 任意阶贝塞尔曲线 
- [x] 样条线 均匀B样条线  
- [x] 实现曲线编辑
- [x] 圆(正负画圆 中点画圆 Bresenham画圆) 
- [x] 椭圆(正负法 中点法)   
- [x] 空心三角形  
- [x] 重心坐标绘制颜色渐变三角形 颜色插值）

### 2. 3D 图形学

- [x] 摄像机 投影 视口矩阵推导
- [x] 3D模型加载
- [x] 将摄像机 MeshRender Transform 等改造成组件进行加载
- [x] 视景体裁剪
- [x] 背面拣选
- [x] 近截面裁剪

### 3. Shader 着色器



# 演示

画线

![](/images/DrawLine.gif)
![](/images/ColorLerpDrawLine.gif)
![](/images/ColorLerpDrawLine2.gif)

矩形

![](/images/DrawRect.gif)

圆

![](/images/DrawCircle.png)

椭圆 

![](/images/ellipse.png)


三角形

![](/images/Triangle.gif)


曲线

![](/images/BezierCurve.png)
![](/images/BezierCurveControl.png)

样条曲线

![](/images/Spline_BSpine.png)


裁剪

![](/images/Cull.gif)
![](/images/Cull2.gif)
![](/images/ClipByNearPlane.png)


MVP

![](/images/Wireframe.gif)


顶点插值

![](/images/TriangleColorLerp.png)
![](/images/Palette.png)


深度缓冲和UV

![](/images/UVAndDepthBuffer.gif)

模板缓冲

![](/images/StencialBuffer.png)


平行光和点光源

![](/images/Lights.png)


地形渲染和高度图


![](/images/Hightmap.gif)
