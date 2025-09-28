#version 330 core
in vec2 TexCoord;
uniform sampler2D inputTexture;

uniform float time;
uniform float strength;
uniform float speed;

out vec4 FragColor;

float getFps() { 
    return 15.0 * speed / 100.0 + 15.0; 
}

float getIntensity() { 
    return 0.04 * strength / 100.0; 
}

vec2 getDirection() {
  vec2 directions[5];
  directions[0] = vec2(-1.0, -1.0);
  directions[1] = vec2(-1.0, 1.0);
  directions[2] = vec2(1.0, 1.0);
  directions[3] = vec2(1.0, -1.0);
  directions[4] = vec2(0.0, 0.0);

  int id = int(floor(getFps() * time));
  id = id % 10 + 1;
  if (id > 5) {
    id = 10 - id;
    if (id == 0) {
      id = 5;
    }
  }
  return directions[id] * getIntensity();
}

void main() {
  vec2 coord = TexCoord + getDirection();
  FragColor = texture(inputTexture, coord);
}