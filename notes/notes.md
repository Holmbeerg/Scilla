# Notes related to OpenGL and graphics programming in general. Primary source for now is learnopengl.com

## **CLion markdown shortcuts**
`Ctrl + B:` Bold<br>
`Code (like this):` Ctrl + Shift + C<br>
`Insert image:` Ctrl + U<br>
`Insert link:` Ctrl + Shift + U<br>
TeX and LaTeX syntax math supported with dollarsigns 

---
## **Vertex Array Objects (VAO)**
To use a VAO all you have to do is bind the VAO using `glBindVertexArray`. From that point on we should bind/configure the corresponding VBO(s) and attribute pointer(s) and then unbind the VAO for later use.

VAO stores our vertex attribute configuration and which VBO to use. Usually when you have multiple objects you want to draw, you first generate/configure all the VAOs (and thus the required VBO and attribute pointers) and store those for later use. The moment we want to draw one of our objects, we take the corresponding VAO, bind it, then draw the object and unbind the VAO again.

![](https://learnopengl.com/img/getting-started/vertex_array_objects.png)
---

## **Element Buffer Objects (EBO)**
Indexed drawing, using indices and only need to specify unique vertices.  
The last element buffer object that gets bound while a VAO is bound is stored as the VAO's element buffer object. Binding to a VAO then also automatically binds that EBO.

![](https://learnopengl.com/img/getting-started/vertex_array_objects_ebo.png)
---

## **Rasterization and Fragment Interpolation**
During rasterization, the graphics pipeline converts vector graphics (triangles) into a raster image (pixels/fragments).

The rasterization stage usually results in a lot more fragments than vertices specified. The rasterizer determines the position of those fragments based on where they reside on the triangle. Based on this position, it interpolates all the fragment shader's input variables.

Interpolation is the act of calculating smooth, intermediate values (data points) between known points based on their relative positions. In graphics, it involves determining the value of a specific attribute (like color, texture coordinates, or normals) for each fragment within a primitive by mathematically blending the values from the primitive's vertices

Interpolation in OpenGL occurs after the vertex shader and before the fragment shader, during the rasterization stage. When you pass data from the 
vertex shader to the fragment shader, like texture coordinates, OpenGL performs interpolation based on the barycentric coordinates of the fragments.

determines the pixels covered by a primitive (e.g. a triangle) and interpolates the output parameters of the vertex shader

When OpenGL rasterizes a triangle, it divides the triangle into fragments (potential pixels), and for each fragment, it needs to interpolate the vertex attributes (like texture coordinates or colors) between the three vertices of the triangle

For example:  
We have a line where the upper point has a green color and the lower point a blue color. If the fragment shader is run at a fragment that resides around a position at 70% of the line, its resulting color input attribute would then be a linear combination of green and blue. To be more precise: 30% blue and 70% green.

![](https://opentk.net/learn/chapter1/img/2-pipeline.png)

Geometry shader is completely optional. Tesselation is also optional.

---

## **Sampling**
Retrieving texture color by using interpolated texture coordinates. In 2D texture images, texture coordinates range from 0 to 1 in both the y and x-axis, with `(0, 0)` being the bottom-left corner.  
Pass 3 texture coordinates as a vertex attribute to the vertex shader. The output of these then get interpolated (if you dont change something with interpolation qualifier) during rasterization and the fragment shader receives the interpolated texture coordinates.

GLSL has built-in function **texture** function to sample a color from a texture. Takes a texture sampler (like sampler2D) as first argument
and corresponding texture coordinates as second.

Example:
```
void main() {
    FragColor = texture(ourTexture, TexCoord);
}
```

---

## **Texture Wrapping**
- **`s`, `t` axis = x, y (width, height)**
- **`r` (z)** if using 3D texture.

If we specify coordinates for the texture outside the range **(0, 0)** to **(1, 1)** the texture will wrap. The result of this depends
on the wrapping method specified for the texture, for example with`glTexParameteri`

Example:
```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
```
`GL_REPEAT`is the default behavior for textures.

![](https://learnopengl.com/img/getting-started/texture_wrapping.png)

---

## **Texture Filtering**
Texture coordinates do not depend on resolution but can be any floating point value. This means that OpenGL has to figure out which texture pixel (**Texel**) to map the (interpolated) texture coordinate to. This is especially important with very large objects with low-resolution textures. This is where **texture filtering** comes in.

### **Options:**
- **GL_NEAREST / Nearest Neighbour / Point Filtering:**  
  Default. Selects the texel whose center is closest to the texture coordinate.<br>
![](https://learnopengl.com/img/getting-started/filter_nearest.png)

- **GL_LINEAR / Bilinear Filtering:**  
  Takes an interpolated value from the texture coordinate's neighboring texels,  approximating a color between the texels. 
The closer a texel's center is to the coordinate, the more that texel's color contributes.<br>
![](https://learnopengl.com/img/getting-started/filter_linear.png)

![](https://learnopengl.com/img/getting-started/texture_filtering.png)

We can set texture filtering for **magnifying** and **minifying** operations (when scaling up or downwards).
In this example we use nearest neighbour for downscaling and linear for upscaling.
```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NEAREST)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```
---

# **Texture Units**
With `glUniform1i` we can assign a **location** value to the texture sampler so we can set multiple textures at once
in a fragment shader. The **location** of a texture, can think like a container, is more commonly known as a **Texture Unit**. Default texture unit 
is 0 which is the default active texture. Not all graphics drivers assign a default texture. 

Main purpose of **Texture Units** is to allow us to use more than one texture in a shader. By assigning **Texture Units** (locations) to the 
samplers, we can bind to multiple textures at once, as long as we activate the corresponding texture unit first.

Can activate **Texture Units** like this:
```
glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
glBindTexture(GL_TEXTURE_2D, texture);
```
`GL_TEXTURE0` is always active by default. OpenGL should have 16 **Texture Units** we can use. If we want to add another texture
we need to add 1 more sampler in the fragment shader.
```
#version 460 core
...

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}

```

**mix** is a built-in GLSL function to mix textures. Takes two textures with texture coordinate and **linearly interpolates** between them
based on the third argument. The third argument decide the specific values. If last value is 0.2, the final color will be a mix
of **80%** of the first value and **20%** of the second. If its 1, it will be **0%** of the first value and **100%** of the second.
---
# **Mipmaps**
### **Example Issue:**
Objects far away have the same high-resolution texture as objects close by. 
Fragments span a larger portion of the texture, leading to difficulty picking 
a texture color and thus retrieving the right color value. 
This produces visible artifacts on small objects and unnecessarily 
uses memory bandwidth by applying high-resolution textures on small objects.

### **Solution: Mipmaps**
![Mipmap](https://learnopengl.com/img/getting-started/mipmaps.png)

Mipmaps are a collection of texture images where each subsequent texture is 
**half the size** of the previous one. The further away an object is, the 
smaller the texture that should be used. Mipmap level `0` is the original texture, `1` is the next largest and so on.


When switching between mipmap levels during rendering, OpenGL can produce some artifacts like sharp edges between two mipmap layers. Just as with texture filtering we can 
filter between mipmap levels using **NEAREST** and **LINEAR**<br>
<br>
Common mistake is to set one of the mipmap filtering options as the magnification filter, doesnt
have any effect since mipmaps are primarily used when textures are downscaled. Texture magnification doesnt
use mipmaps and giving it a mipmap filtering option will generate `GL_INVALID_ENUM` error.
<br><br>
To load an image we use [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h). We later use this data loaded in `glTexImage2D`.

```
int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
```

After generating a texture with [glGenTextures](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenTextures.xhtml) 
 binding with [glBindTexture](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindTexture.xhtml) and setting [parameters](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml)
we can call [glTexImage2D](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml).
After this the currently bound texture object now has the texture image attached to it. It only has the base-level of the texture attached though,
so if we don't want to specify all the different images (mipmap levels) manually we can call `glGenerateMipmap(GL_TEXTURE_2D);`
This automatically generates all the required mipmaps for the currently bound texture.

Good practice to free image memory after generating texture and mipmaps
```
stbi_image_free(data);
```
# **Applying textures**
As with other vertex attribute, adding to vertices (with the new texture coordinates), we need to notify OpenGL of the new attribute and format with `glVertexAttribPointer`,
and not forget to adjust stride length. Activate attribute with `glEnableVertexAttribArray`. Also need to adjust vertex shader to accept
texture coordinates as a vertex attribute with for example 
```
layout (location = 2) in vec2 aTexCoord;
out vec2 TexCoord;
```
Fragment shader should then accept TexCoord output variable as an input variable. Fragment shader should also have access to the texture
object we created. GLSL has built-in data-type for texture objects called sampler, postfix of specific type (1D, 2D or 3D).
Can then add a texture to the fragment shader by declaring 
```
uniform sampler2D ourTexture;
```
then pass this in to GLSL function. Example:
```
#version 460 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    FragColor = texture(ourTexture, TexCoord);
}
``` 
Texture function in GLSL samples the corresponding color value using the set texture parameters. Output of the fragment 
shader is then the **filtered** color at the **interpolated** texture coordinate

Example:
```
glBindVertexArray(VAO);
glBindTexture(GL_TEXTURE_2D, texture);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

---

