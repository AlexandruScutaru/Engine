#version 330 core

//out vec4 FragColor;


void main(){             
	//FragColor = glm::vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
	//FragColor = glm::vec4(0.0, 0.0, 1.0, 1.0);
	gl_FragDepth = gl_FragCoord.z;
} 