#version 330

out vec2 moments;

void main() {

	moments = vec2(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z);

}
