#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Shader_Animated
{
public:
	Shader_Animated();
	~Shader_Animated();
	unsigned int getShaderProgramID();

private:
	unsigned int shaderProgram = NULL;
};


Shader_Animated::Shader_Animated()
{


    /*
    GLSL vertex shaderand fragment shader
    */
    const char* vertexshaderGLSL = R"glsl(
                #version 450 core

                layout(location = 0) in vec3 modelPos;
                layout(location = 2) in vec3 normalsDataIn;
                layout(location = 3) in vec2 textCoordsIn;
                layout(location = 4) in ivec4 boneIds;
                layout(location = 5) in vec4 weights;

                uniform mat4 Model;
                uniform mat4 View;
                uniform mat4 Projection;

                const int MAX_BONES = 5;
                const int MAX_BONE_INFLUENCE = 4;                
                
                uniform mat4 finalBonesMatrices[MAX_BONES];

                out vec2 textCoords;

                void main()
                {
                    vec4 totalPosition = vec4(0.f);
                    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
                    {
                        if(boneIds[i] == -1)
                        {
                            continue;
                        }
                        if(boneIds[i] >= MAX_BONES)
                        {
                            totalPosition = vec4(modelPos, 1.0f);
                            break;
                        }
                        
                        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(modelPos, 1.0f);
                        totalPosition += localPosition * weights[i];
                        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * normalsDataIn;
                    }

                    mat4 viewModel = View * Model;
                    gl_Position = Projection * viewModel * totalPosition;
                    textCoords = textCoordsIn;
                    
                }
                )glsl";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexshaderGLSL, NULL);
    glCompileShader(vertexShader);

    const char* fragmentshaderGLSL = R"glsl(
                 #version 450 core

                in vec3 vertexNormals;
                in vec3 fragPos;
                in vec2 textCoords;

                out vec4 FragColor;

                uniform vec3 eyePos;
               
                struct Material {
                    float shininess;
                    float refract_indice;
                    float reflectivity;

                    vec3 ambient;
                    vec3 reflective;
                    vec3 diffuse;
                    vec3 specular;
                };
                uniform Material material;

                struct Light {
                    vec3 diffuse;
                    vec3 specular;
                    vec3 position;
                    vec3 ambient;
                };
                uniform Light light;               
            
                uniform sampler2D texture0;



                void main()
                {
                    // texture
                    vec3 texture = vec3(texture(texture0, textCoords));


                    // AMBIENT
                    
                    vec3 ambient = light.ambient + material.ambient;
                    
                    /* ******************* */                    
                    
                    
                    // DIFFUSE

                    vec3 norm = normalize(vertexNormals);
                    vec3 lightDir = normalize(light.position - fragPos);
                    float diffuseDot = max(dot( norm, lightDir ),  0.0f );            
                    vec3 diffuse = (light.diffuse * diffuseDot) + material.diffuse;

                    /* ******************** */


                    // SPECULAR

                    vec3 eyeNormal = normalize(eyePos - fragPos);
                    vec3 R = -reflect(-lightDir, eyeNormal);

                    float angle = max(dot(eyeNormal, R), 0.0f);
                    float spec = pow(angle, material.shininess);
                    vec3 specular = (light.specular * spec) + material.specular;

                    /* ******************** */

                    // result
                    vec3 result = (ambient + diffuse + specular) + texture;
	                FragColor = vec4(result, 1.0f);
                }
        )glsl";


    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentshaderGLSL, NULL);
    glCompileShader(fragmentShader);


    /* CHECK SHADER COMPILE ERROR */

    GLint isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        glDeleteShader(fragmentShader);
        printf("FragmentShader Error\n");
        exit(2);
    }

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        glDeleteShader(vertexShader);
        printf("VertexShader Error\n");
        exit(2);
    }


    /* ********************************************************** */


    // Attach and link shader
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* ***************************** */
}

unsigned int Shader_Animated::getShaderProgramID()
{
	return shaderProgram;
}

Shader_Animated::~Shader_Animated()
{

}