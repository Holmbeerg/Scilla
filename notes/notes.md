# Notes related to OpenGL and graphics programming in general. Primary source for now is learnopengl.com

## **CLion things, markdown shortcuts etc**
`Ctrl + B:` Bold<br>
`Code (like this):` Ctrl + Shift + C<br>
`Insert image:` Ctrl + U<br>
`Insert link:` Ctrl + Shift + U<br>
TeX and LaTeX syntax math supported with dollarsigns

To get relative paths to work: run -> edit configurations and set the working directory to project root

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
# **Some math**
## **Vectors**
A vector has a direction and a magnitude (also known as its strength or length). A 2D vector exists in a flat plane, like a computer screen or a piece of paper.

![](https://learnopengl.com/img/getting-started/vectors.png)

3D vectors are representations of three-dimensional space and we add a Z-axis.
In formulas it is common for vectors to be displayed as:

$\bar{v}=\left(\begin{array}{l}
x \\
y \\
z
\end{array}\right)$

If we want to visualize vectors as positions we can imagine the origin of the direction vector to be (0,0,0) and then point towards a certain direction that specifies the point, making it a position vector (we could also specify a different origin and then say: 'this vector points to that point in space from this origin').
The position vector (3,5) would then point to (3,5) on the graph with an origin of (0,0). Using vectors we can thus describe directions and positions in 2D and 3D space. 

A scalar is a single digit. When adding/subtracting/multiplying or dividing a vector with a scalar we simply add/subtract/multiply or divide each element of the vector by the scalar. For addition it would look like this: 

$
\left(\begin{array}{l}
1 \\
2 \\
3
\end{array}\right)+x \rightarrow\left(\begin{array}{l}
1 \\
2 \\
3
\end{array}\right)+\left(\begin{array}{l}
x \\
x \\
x
\end{array}\right)=\left(\begin{array}{l}
1+x \\
2+x \\
3+x
\end{array}\right)
$

Where + can be +,−,⋅ or ÷ where ⋅ is the multiplication operator.

### Addition of two vectors:
$
\bar{v}=\left(\begin{array}{l}
1 \\
2 \\
3
\end{array}\right), \bar{k}=\left(\begin{array}{l}
4 \\
5 \\
6
\end{array}\right) \rightarrow \bar{v}+\bar{k}=\left(\begin{array}{l}
1+4 \\
2+5 \\
3+6
\end{array}\right)=\left(\begin{array}{l}
5 \\
7 \\
9
\end{array}\right)
$

Addition of vectors v=(4,2) and k=(1,2)

![](https://learnopengl.com/img/getting-started/vectors_addition.png)

Just like normal addition and subtraction, vector subtraction is the same as addition with a negated second vector:

$
\bar{v}=\left(\begin{array}{l}
1 \\
2 \\
3
\end{array}\right), \bar{k}=\left(\begin{array}{l}
4 \\
5 \\
6
\end{array}\right) \rightarrow \bar{v}+-\bar{k}=\left(\begin{array}{l}
1+(-4) \\
2+(-5) \\
3+(-6)
\end{array}\right)=\left(\begin{array}{l}
-3 \\
-3 \\
-3
\end{array}\right)
$

Subtracting two vectors from each other results in a vector that's the difference of the positions both vectors are pointing at. This proves useful in certain cases where we need to retrieve a vector that's the difference between two points. 

![](https://learnopengl.com/img/getting-started/vectors_subtraction.png)

### Length/Magnitude of vector 
The length of a vector is always positive. To retrieve the **length/magnitude** of a vector we use **Pythagoras theorem**:

![](https://learnopengl.com/img/getting-started/vectors_triangle.png)

$
\|\bar{v}\|=\sqrt{x^2+y^2}
$

**$\|\bar{v}\|$** is denoted as the length of vector $\bar{v}$. Can extend to 3D by adding $z^2$ to the equation.

a **Unit Vector** is a vector with a length of 1. We can calculate a unit vector $\hat{n}$ from any vector by dividing each of the vector's components by its length:

$
\hat{n}=\frac{\bar{v}}{\|\bar{v}\|}
$

We call this normalizing a vector. Unit vectors are displayed with a little roof over their head and are generally easier to work with, especially when we only care about their directions (the direction does not change if we change a vector's length). 

### **Dot product** (skalärprodukt) of two vectors:

$
\bar{v} \cdot \bar{k}=\|\bar{v}\| \cdot\|\bar{k}\| \cdot \cos \theta
$ 

where $\cos\theta$ is the angle between the vectors. If the two vectors were **Unit Vectors** we would have

$
\hat{v} \cdot \hat{k}=1 \cdot 1 \cdot \cos \theta=\cos \theta
$

Now the **Dot Product** only defines the angle between the two vectors.

$\cos (90^{\circ })$ is equal to 0 and $\cos (0^{\circ })$ is equal to 1. This allows us to test
if the two vectors are **orthogonal** or **parallel** to each other using the **Dot Product**.
**Orthogonal** means the vectors are at a **right-angle** to each other (right angle is exactly $90^{\circ }$). Dot product becomes very useful when doing lighting calculations.

So how do we calculate the dot product? The dot product is a component-wise multiplication where we add the results together. It looks like this with two unit vectors:

$
\left(\begin{array}{c}
0.6 \\
-0.8 \\
0
\end{array}\right) \cdot\left(\begin{array}{l}
0 \\
1 \\
0
\end{array}\right)=(0.6 * 0)+(-0.8 * 1)+(0 * 0)=-0.8
$

To get the degree between the two unit vectors we use the inverse of the cosine function (arccos). 

${cos^{-1}(-0.8)} = 143^{\circ}$

### Crossproduct
Crossproduct is only defined in 3D space and takes two non-parallel vectors as input and produces a third vector 
that is orthogonal to both the input vectors. If both the input vectors are orthogonal to each other as well, then a cross product
would result in 3 orthogonal vectors. How it looks (v x k):

![](https://learnopengl.com/img/getting-started/vectors_crossproduct.png)

cross product between two orthogonal vectors A and B: 

$ \begin{pmatrix}
A_x \\
A_y \\
A_z
\end{pmatrix}
\times
\begin{pmatrix}
B_x \\
B_y \\
B_z
\end{pmatrix}
=
\begin{pmatrix}
A_y \cdot B_z - A_z \cdot B_y \\
A_z \cdot B_x - A_x \cdot B_z \\
A_x \cdot B_y - A_y \cdot B_x
\end{pmatrix}
$

### Matrices
Rectangular array of numbers, symbols and/or mathematical expressions. Each individual item in a matrix is called
an **element** of the matrix. Example, 2x3 matrix:

$
\begin{bmatrix}
1 & 2 & 3\\
4 & 5 & 6
\end{bmatrix}
$

Matrices are indexed by (i,j) where i is the row and j is the column. 2x3 is the **_dimensions_** of the matrix, 2 rows and 3 columns.

**Addition** and **Subtraction** is only defined for matrices of the same dimensions. Example:

$
\begin{bmatrix}
1 & 2 \\
3 & 4
\end{bmatrix}+
\begin{bmatrix}
5 & 6 \\
7 & 8
\end{bmatrix}=
\begin{bmatrix}
1+5 & 2+6 \\
3+7 & 4+8
\end{bmatrix}=
\begin{bmatrix}
6 & 8 \\
10 & 12
\end{bmatrix}
$

Same rules for subtraction.

### Matrix-Matrix multiplication
Rules:
* You can only multiply two matrices if the number of columns on the left-hand side matrix is equal to the number of rows on the right-hand side matrix
* Matrix multiplication is not commutative that is A⋅B≠B⋅A.

Example:

$
\begin{bmatrix}
1 & 2 \\
3 & 4
\end{bmatrix}\cdot
\begin{bmatrix}
5 & 6 \\
7 & 8
\end{bmatrix}=
\begin{bmatrix}
1\cdot5+2\cdot7 & 1\cdot6+2\cdot8 \\
3\cdot5+4\cdot7 & 3\cdot6+4\cdot8
\end{bmatrix}=
\begin{bmatrix}
19 & 22 \\
43 & 50
\end{bmatrix}
$

$
\begin{gathered}
\begin{bmatrix}
4 & 2 & 0 \\
0 & 8 & 1 \\
0 & 1 & 0
\end{bmatrix}\cdot
\begin{bmatrix}
4 & 2 & 1 \\
2 & 0 & 4 \\
9 & 4 & 2
\end{bmatrix}=
\begin{bmatrix}
4\cdot4+2\cdot2+0\cdot9 & 4\cdot2+2\cdot0+0\cdot4 & 4\cdot1+2\cdot4+0\cdot2 \\
0\cdot4+8\cdot2+1\cdot9 & 0\cdot2+8\cdot0+1\cdot4 & 0\cdot1+8\cdot4+1\cdot2 \\
0\cdot4+1\cdot2+0\cdot9 & 0\cdot2+1\cdot0+0\cdot4 & 0\cdot1+1\cdot4+0\cdot2
\end{bmatrix} \\
=
\begin{bmatrix}
20 & 8 & 12 \\
25 & 4 & 34 \\
2 & 0 & 4
\end{bmatrix}
\end{gathered}
$

Matrix multiplication is a combination of normal multiplication and addition using the left-matrix's rows with the right-matrix's columns.

### Matrix-Vector multiplication
A vector is basically a **Nx1** matrix where **N** is the vector's number of **components** (also known as an **N-dimensional vector**).
Vectors are just like matrices an array of numbers, but with only 1 column.

If we have a **M**x**N** matrix we can multiply this with the vector **N**x**1**, since the columns on the left is equal to the rows on the right.

There are lots of interesting 2D/3D transformations we can place inside a matrix, and multiplying that matrix with a vector then **_transforms_** that vector.

### Identity matrix
In OpenGL we usually work with **4x4** transformation matrices for several reasons, of them being that most vectors are of size 4
(vec4). The most simple **transformation matrix** is the **identity matrix**. Leaves vector unharmed.

$
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}\cdot
\begin{bmatrix}
1 \\
2 \\
3 \\
4
\end{bmatrix}=
\begin{bmatrix}
1\cdot1 \\
1\cdot2 \\
1\cdot3 \\
1\cdot4
\end{bmatrix}=
\begin{bmatrix}
1 \\
2 \\
3 \\
4
\end{bmatrix}
$

Even if it doesnt **transform**, the **identity matrix** is usually a starting point for generating other transformation matrices. Deeper into linear algebra it is very useful for proving theorems and solving linear equations.

### Scaling
![](https://learnopengl.com/img/getting-started/vectors_scale.png)

Scaling the vector $\bar{v}=(3,2).$ by (0.5, 2.0). This scaling operation is a **non-uniform** scale, since the scaling factor is not the same for each axis. If it was, it would be called a **uniform-scale**. OpenGL usually operates in 3D space, we could set z-axis scale to 1, leaving it unharmed.

If we represent the scaling variables as $({S_1}, {S_2}, {S_3})$ we can define a scaling matrix on any vector (_x,y,z_) as: 

$
\begin{gathered}
\begin{bmatrix}
S_1 & 0 & 0 & 0 \\
0 & S_2 & 0 & 0 \\
0 & 0 & S_3 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}\cdot
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}=
\begin{pmatrix}
S_1\cdot x \\
S_2\cdot y \\
S_3\cdot z \\
1
\end{pmatrix}
\end{gathered}
$

4th scaling value is set to 1 for now, the **w** component.

**W** component of a vector is known as a **Homogeneous coordinate**. Homogeneous coordinates add an extra dimension called **W**, which scales the X, Y, and Z dimensions. Matrices for translation and perspective projection transformations can only be applied to homogeneous coordinates, 
which is why they are so common in 3D computer graphics. Value of **w** affects the size (aka scale) of the image.
To get the 3D vector from a homogeneous vector we divide the x, y and z coordinate by its **w** coordinate. This is called **perspective division**.

A lot of the time the initial value of **w** is set to 1, so we don't notice it.

![](https://www.tomdalling.com/images/posts/explaining-homogeneous-coordinates/projector_xyw.png)

### Translation
**Translation** is the process of adding another vector on top of the original vector to return a new vector with a **different position**, thus moving the vector based on a **translation vector**.
Just like the scaling matrix there are several locations on a 4-by-4 matrix that we can use to perform certain operations and for translation those are the top-3 values of the 4th column.


If we represent the translation vector as $({T_x}, {T_y}, {T_z})$ we can define the translation matrix by: 

$\begin{bmatrix}
1 & 0 & 0 & T_x \\
0 & 1 & 0 & T_y \\
0 & 0 & 1 & T_z \\
0 & 0 & 0 & 1
\end{bmatrix}\cdot
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}=
\begin{pmatrix}
x+T_x \\
y+T_y \\
z+T_z \\
1
\end{pmatrix}
$

### Rotation
A rotation in 2D or 3D is represented with an angle, either in degrees or in radians.  
Rotations in 3D are specified with an angle and a **_rotation axis_**.
Angle specified will rotate the object along the rotation axis given.

Using trigonometry it is possible to transform vectors to newly rotated vectors given an angle

![](https://learnopengl.com/img/getting-started/vectors_angle.png)

Rotation around the X-axis

![](https://opentk.net/learn/chapter1/img/7-latex_matrix_rotation_x.png)

Rotation around the Y-axis

![](https://opentk.net/learn/chapter1/img/7-latex_matrix_rotation_y.png)

Rotation around the Z-axis

![](https://opentk.net/learn/chapter1/img/7-latex_matrix_rotation_z.png)

With these rotation matrices we can transform our position vectors around one of the three unit axes. To rotate around arbitary 3D axis we can combine them by rotating around each one, 
but this introduces a problem called **Gimbal lock**. **Gimbal lock** means you lose a degree of freedom and rotation becomes constrained.

It is better to rotate around an arbitary unit axis, for example (0.662,0.2,0.722), which is a unit vector, right away instead
of combining the rotation matrices:

![](https://opentk.net/learn/chapter1/img/7-latex_matrix_rotation_all.png)

To truly prevent **Gimbal locks** we have to represent rotations using **quaternions**, that except from being safer are more
computionally friendly.
 
### Combining matrices
True power from using matrices for transformations is that we can combine multiple transformations in single matrix thanks
to **Matrix-Matrix** multiplication.

Say we have a vector (x,y,z) and we want to scale it by 2 and then translate it by (1,2,3). We need a **translation** and a **scaling matrix** for our required steps. The resulting transformation matrix would then look like:

$
\begin{gathered}
Trans.Scale=
\begin{bmatrix}
1 & 0 & 0 & 1 \\
0 & 1 & 0 & 2 \\
0 & 0 & 1 & 3 \\
0 & 0 & 0 & 1
\end{bmatrix}.
\begin{bmatrix}
2 & 0 & 0 & 0 \\
0 & 2 & 0 & 0 \\
0 & 0 & 2 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}=
\begin{bmatrix}
2 & 0 & 0 & 1 \\
0 & 2 & 0 & 2 \\
0 & 0 & 2 & 3 \\
0 & 0 & 0 & 1
\end{bmatrix}
\end{gathered}
$

Note that we first have the **translation** matrix and then the **scale transformation** matrix when multiplying matrices. As **Matrix multiplication** is **NOT** commutative, the order is important.
This is because of the fact that when multiplying matrices the **right-most matrix** is applied/multiplied to the vector/point **FIRST** so you should read the multiplications from **right to left**.
In this example scaling is on the right so it is applied first and then translation.

$
\begin{aligned}
AB=
\begin{bmatrix}
1 & 1 \\
0 & 0
\end{bmatrix}
\begin{bmatrix}
1 & 0 \\
0 & 0
\end{bmatrix}=
\begin{bmatrix}
1 & 0 \\
0 & 0
\end{bmatrix} 
\\
\\
BA=
\begin{bmatrix}
1 & 0 \\
0 & 0
\end{bmatrix}
\begin{bmatrix}
1 & 1 \\
0 & 0
\end{bmatrix}=
\begin{bmatrix}
1 & 1 \\
0 & 0
\end{bmatrix}
\end{aligned}
$

### Important 
It is advised to first do scaling operations, then rotations and lastly translations when combining matrices otherwise they may (negatively) affect each other. For example, if you would first do a translation and then scale, the translation vector would also scale!

Running the final transformation matrix on our vector results in the following vector: 

$
\begin{bmatrix}
2 & 0 & 0 & 1 \\
0 & 2 & 0 & 2 \\
0 & 0 & 2 & 3 \\
0 & 0 & 0 & 1
\end{bmatrix}.
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}=
\begin{bmatrix}
2x+1 \\
2y+2 \\
2z+3 \\
1
\end{bmatrix}
$

Resulting vector is first scaled by two and then translated by (1,2,3).



Here we first rotate the container around the origin (0,0,0) and once it's rotated, we translate its
rotated version to the bottom-right corner of the screen. Remember that the actual transformation order
should be read in reverse: even though in code we first translate and then later rotate, the actual transformations
first apply a rotation and then a translation.Here we first rotate the container around the origin (0,0,0) and
once it's rotated, we translate its rotated version to the bottom-right corner of the screen. Remember that the
actual transformation order should be read in reverse: even though in code we first translate and then later rotate, 
the actual transformations first apply a rotation and then a translation.
```
glm::mat4 trans = glm::mat4(1.0f);
trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
GLuint transformLocation = glGetUniformLocation(shaderProgram2.getId(), "transform");
glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
```
