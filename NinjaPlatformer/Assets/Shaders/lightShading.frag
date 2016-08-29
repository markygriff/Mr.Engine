#version 410

//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

void main() {

  //get length of uv vector
  float distance = length(fragmentUV);

  //alpha depends on distance away from texture (fade lighting)
  color = vec4(fragmentColor.r,fragmentColor.g,fragmentColor.b, fragmentColor.a * (pow(0.01, distance) - 0.01));
}
