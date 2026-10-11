#pragma once
const char* fragmentShaderSource = R"(
	#version 330 core
	out vec4 FragColor;
	uniform vec4 uColor;	
	void main(){
	FragColor = uColor;
	};	
)";
