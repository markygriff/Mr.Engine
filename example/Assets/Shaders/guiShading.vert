#version 410

uniform mat4 ProjMtx;

in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexUV;

out vec2 fragmentPosition;
out vec4 fragmentColor;
out vec2 fragmentUV;

void main()
{
  fragmentUV = vertexUV;
  fragmentColor = vertexColor;
  gl_Position = ProjMtx * vec4(vertexPosition.xy,0,1);
}
