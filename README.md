# OpenGLSamples

## 项目简介

此项目是作者在校学习图形学时，制作的OpenGL案例。主要参考了[LearnOpenGL](https://learnopengl-cn.github.io/)代码实现。项目中主要的功能包括：
- **glfw+glad+glm相关OpenGL环境配置**
- **assimp加载模型**
- **opencv摄像头捕获+视频编解码**
- **imgui支持**
- **freetype文字相关**

案例覆盖（48个案例）：
- **LearnOpenGL相关案例。阴影、法线贴图、深度贴图等**
- **光线追踪**
- **后处理**
- **[shaderToy](https://www.shadertoy.com/)、[GLSLSandbox](https://glslsandbox.com/)兼容**
- **OpenCV+Imgui滤镜**
- **分形几何相关**


## 案例
- **[1.创建OpenGL窗口](./samples/1.1-createWindow/)**

<img src="./data/readme/1.1-createWindow.jpg" alt="1.1-createWindow" width="282"/>

- **[2.绘制简单三角形](./samples/1.2-helloTriangle/)**

<img src="./data/readme/1.2-helloTriangle.jpg" alt="1.2-helloTriangle" width="282"/>

- **[3.着色器初试](./samples/1.3-helloShader/)**

<img src="./data/readme/1.3-helloShader.jpg" alt="1.3-helloShader" width="282"/>

- **[4.纹理初试](./samples/1.4-helloTexture/)**

<img src="./data/readme/1.4-helloTexture.jpg" alt="1.4-helloTexture" width="282"/>

- **[5.三维变换](./samples/1.5-helloTransform/)**

<img src="./data/readme/1.5-helloTransform.jpg" alt="1.5-helloTransform" width="282"/>

- **[6.坐标系统](./samples/1.6-helloCoordinateSystem/)**

<img src="./data/readme/1.6-helloCoordinateSystem.jpg" alt="1.6-helloCoordinateSystem" width="282"/>

- **[7.相机](./samples/1.71-helloCamera/)**

<img src="./data/readme/1.71-helloCamera.jpg" alt="1.71-helloCamera" width="282"/>

- **[8.第一人称相机](./samples/1.72-camera-freedom/)**

<img src="./data/readme/1.72-camera-freedom.jpg" alt="1.72-camera-freedom" width="282"/>

- **[9.简单光照](./samples/2.1-sampleLighting/)**

<img src="./data/readme/2.1-sampleLighting.jpg" alt="2.1-sampleLighting" width="282"/>

- **[10.光照贴图](./samples/2.2-lightingTexture/)**

<img src="./data/readme/2.2-lightingTexture.jpg" alt="2.2-lightingTexture" width="282"/>

- **[11.射灯](./samples/2.3-castLiting/)**

<img src="./data/readme/2.3-castLiting.jpg" alt="2.3-castLiting" width="282"/>

- **[12.模型加载](./samples/3.1-modelLoading/)**

<img src="./data/readme/3.1-modelLoading.jpg" alt="3.1-modelLoading" width="282"/>

- **[13.模板测试（描边）](./samples/4.1-stencileTest/)**

<img src="./data/readme/4.1-stencileTest.jpg" alt="4.1-stencileTest" width="282"/>

- **[14.混合](./samples/4.2-blend/)**

<img src="./data/readme/4.2-blend.jpg" alt="4.2-blend" width="282"/>

- **[15.透明贴图](./samples/4.3-translucentTexture/)**

<img src="./data/readme/4.3-translucentTexture.jpg" alt="4.3-translucentTexture" width="282"/>

- **[16.帧缓冲](./samples/4.4-frameBuffer/)**

<img src="./data/readme/4.4-frameBuffer.jpg" alt="4.4-frameBuffer" width="282"/>

- **[17.立方体贴图（天空盒）](./samples/4.5-cubeMap/)**

<img src="./data/readme/4.5-cubeMap.jpg" alt="4.5-cubeMap" width="282"/>

- **[18.几何着色器](./samples/4.6-geometryShader/)**

<img src="./data/readme/4.6-geometryShader.jpg" alt="4.6-geometryShader" width="282"/>

- **[19.实例化（草地）](./samples/4.7-instantiation/)**

<img src="./data/readme/4.7-instantiation.jpg" alt="4.7-instantiation" width="282"/>

- **[20.抗锯齿](./samples/4.8-antiAliasing/)**

<img src="./data/readme/4.8-antiAliasing.jpg" alt="4.8-antiAliasing" width="282"/>

- **[21.Blinn-Phong光照模型](./samples/5.1-Blinn-Phong/)**

<img src="./data/readme/5.1-Blinn-Phong.jpg" alt="5.1-Blinn-Phong" width="282"/>

- **[22.Gamma矫正](./samples/5.2-GammaCorrect/)**

<img src="./data/readme/5.2-GammaCorrect.jpg" alt="5.2-GammaCorrect" width="282"/>

- **[23.点阴影](./samples/5.3.1-pointShadow/)**

<img src="./data/readme/5.3.1-pointShadow.jpg" alt="5.3.1-pointShadow" width="282"/>

- **[24.万向阴影](./samples/5.3.2-universalShadow/)**

<img src="./data/readme/5.3.2-universalShadow.jpg" alt="5.3.2-universalShadow" width="282"/>

- **[25.法线贴图](./samples/5.4-normalMap/)**

<img src="./data/readme/5.4-normalMap.jpg" alt="5.4-normalMap" width="282"/>

- **[26.视差贴图](./samples/5.5-parallaxMapping/)**

<img src="./data/readme/5.5-parallaxMapping.jpg" alt="5.5-parallaxMapping" width="282"/>

- **[27.HDR](./samples/5.6-HDR/)**

<img src="./data/readme/5.6-HDR.jpg" alt="5.6-HDR" width="282"/>

- **[28.眩光](./samples/5.7-Bloom/)**

<img src="./data/readme/5.7-Bloom.jpg" alt="5.7-Bloom" width="282"/>

- **[29.延迟渲染](./samples/5.8-deferredShading/)**

<img src="./data/readme/5.8-deferredShading.jpg" alt="5.8-deferredShading" width="282"/>

- **[30.SSAO（屏幕空间环境光遮蔽）](./samples/5.9-ssao/)**

<img src="./data/readme/5.9-ssao.jpg" alt="5.9-ssao" width="282"/>

- **[31.光线追踪（球+立方体）](./samples/6.1-rayTracing/)**

<img src="./data/readme/6.1-rayTracing.jpg" alt="6.1-rayTracing" width="282"/>

- **[32.光线追踪（球，三维谢尔宾斯三角形）](./samples/6.2-rayTracing-Scherbinski_gasket/)**

<img src="./data/readme/6.2-rayTracing-Scherbinski_gasket.jpg" alt="6.2-rayTracing-Scherbinski_gasket" width="282"/>

- **[33.文字渲染）](./samples/7.1-text/)**

<img src="./data/readme/7.1-text.jpg" alt="7.1-text" width="282"/>

## 案例赏析

- **[1.朱莉亚集（Julia-set：分形几何）](./samples/caseForFun/Julia-Set/)**

<img src="./data/readme/Julia-Set.jpg" alt="Julia-Set" width="282"/>

- **[2.Mandelbrot-Set（分形几何）](./samples/caseForFun/Mandelbrot-Set/)**

<img src="./data/readme/Mandelbrot-Set.jpg" alt="Mandelbrot-Set" width="282"/>

- **[3.Worley噪声（分形几何）](./samples/caseForFun/Worley-Noise/)**

<img src="./data/readme/Worley-Noise.jpg" alt="Worley-Noise" width="282"/>

- **[4.分形云](./samples/caseForFun/fractalCloud/)**

<img src="./data/readme/fractalCloud.jpg" alt="fractalCloud" width="282"/>

- **[5.分形山](./samples/caseForFun/fractalMountain/)**

<img src="./data/readme/fractalMountain.jpg" alt="fractalMountain" width="282"/>

- **[6.洛伦兹吸引子（混沌理论）](./samples/caseForFun/Lorentz-Attractor/)**

<img src="./data/readme/Lorentz-Attractor.jpg" alt="Lorentz-Attractor" width="282"/>

- **[7.字符画（OpenCV+OpenGL+Imgui）](./samples/caseForFun/Lorentz-Attractor/)**

<img src="./data/readme/OpenGL+OpenCV.jpg" alt="OpenGL+OpenCV" width="282"/>

- **[8.星系（GLSL Sandbox）](./samples/caseForFun/galaxy/)**

<img src="./data/readme/galaxy.jpg" alt="galaxy" width="282"/>

- **[9.imgui测试](./samples/caseForFun/imguiTest/)**

<img src="./data/readme/imguiTest.jpg" alt="imguiTest" width="282"/>

- **[10.路径追踪（GLSL Sandbox）](./samples/caseForFun/rayCasting/)**

<img src="./data/readme/rayCasting.jpg" alt="rayCasting" width="282"/>

- **[11.土星](./samples/caseForFun/saturn/)**

<img src="./data/readme/saturn.jpg" alt="saturn" width="282"/>

- **[12.海洋](./samples/caseForFun/ocean/)**

<img src="./data/readme/ocean.jpg" alt="ocean" width="282"/>

- **[13.独孤信印章-视差贴图](./samples/caseForFun/stamp-parallaxMapping/)**

<img src="./data/readme/stamp-parallaxMapping.jpg" alt="stamp-parallaxMapping" width="282"/>

- **[14.独孤信印章](./samples/caseForFun/stamp/)**

<img src="./data/readme/stamp.jpg" alt="stamp" width="282"/>

- **[15.滤镜测试](./samples/caseForFun/testFilter/)**

<img src="./data/readme/testFilter.jpg" alt="testFilter" width="282"/>

## 编译
- **依赖CMake**

```
1.cd OpenGLSamples/
2.mkdir build
3.cd build/
4.cmake ../
5.make
```
编译完后可执行文件在`build/bin/`目录下

## 添加案例
- **在`samples/`下新建文件夹**
- **文件结构：`myTest/src/Test.cpp`**
- **文件结构：此时cmake会扫描添加Test.cpp所在目录下所有`.c、.cpp、.h、.hpp`文件**
- **添加完后重新编译**



