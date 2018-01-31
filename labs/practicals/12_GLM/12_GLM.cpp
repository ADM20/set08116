#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main() {
	vec2 u(1.0f, 0.0f);
	vec2 v(1.0f, 0.0f);
	vec3 w(1.0f,  0.0f,0.0f);
	vec3 x(1.0f, 0.0f, 0.0f);
	vec4 y(1.0f, 0.0f, 0.0f,0.0f);
	vec4 z(1.0f, 0.0f, 0.0f,0.0f);

	vec3 u(vec2(1.0f, 0.0f), 0.0f);
	vec4 v(vec2(1.0f, 0.0f), 0.0f, 0.0f);
	vec2 w(vec3(1.0f, 0.0f, 0.0f));
	vec4 x(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	vec2 y(vec4(1.0f, 0.0f, 0.0f, 0.0f));
	vec3 z(vec4(1.0f, 0.0f, 0.0f, 0.0f));
}