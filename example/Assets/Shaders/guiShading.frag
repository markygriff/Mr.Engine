#version 410

uniform sampler2D Texture;

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

void main()
{

  vec4 textureColor = texture(Texture, fragmentUV);

  //Make crazy colors using time and position!
  color = fragmentColor * textureColor;
}
