[Go back](../)

# Exercise 5
A shader exercise using vertex and fragment shaders to display a terrain. 
Includes Phong and Gouraud lighting models.

## Description
The core goals of this exercise were:
* Shader
* Heightmap
* Phong
* Gouraud

In this exercise we ditched the old fixed function pipline and used the newer programable pipeline with shaders. 
Explicitly, we wrote a custom vertex and fragment shader to display a terrain based on a heightmap.

This exercise needs GLEW and a newer graphics card to work properly.

## Extra feature
We added normalmapping to the shader. To enable normalmapping, press B. This effect is most visible with disabled textures.

## 3rd Party
* [stb_image.h](https://github.com/nothings/stb) was used for image loading.
* The snow and rock texture were from [Texturehaven](https://texturehaven.com/textures/).

## Images
![First Image](img1.png "Scene")
![Second Image](img2.png "Normalmapping")
![Third Image](img3.png "Phong Shading")
![Fourth Image](img4.png "Gouraud Shading")
