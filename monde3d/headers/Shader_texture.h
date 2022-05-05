#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Shader_texture
{
public:
	Shader_texture();
	~Shader_texture();
    unsigned int getShaderProgramID();

private:
    unsigned int shaderProgram = NULL;
};

Shader_texture::Shader_texture()
{


    /*
    GLSL vertex shaderand fragment shader
    */
            const char* vertexshaderGLSL = R"glsl(
                #version 450 core

                layout(location = 0) in vec3 modelPos;
                layout(location = 1) in vec3 colorDataIn;
                layout(location = 2) in vec3 normalsDataIn;
                layout(location = 3) in vec2 textCoordsIn;

                uniform mat4 Model;
                uniform mat4 ViewProjection;

                out vec3 vertexNormals;
                out vec3 fragPos;
                out vec3 colorsVertex;
                out vec2 textCoords;

                void main()
                {
                    gl_Position = ViewProjection * Model * vec4(modelPos, 1.0f);
                    vertexNormals = normalsDataIn;

                    fragPos = vec3(Model * vec4(modelPos, 1.0f));

                    colorsVertex = colorDataIn;
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
                in vec3 colorsVertex;
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

                
                //uniform sampler2D texture0;


                struct Light {
                    vec3 diffuse;
                    vec3 specular;
                    vec3 position;
                    vec3 ambient;
                };
                uniform Light light;               
            



                void main()
                {
                    // texture
                    //vec3 texture = vec3(texture(texture0, textCoords));


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
                    vec3 result = (ambient + diffuse + specular);
	                FragColor = vec4(result, 1.0f);
                }
        )glsl";
        
    // TODO Comprendre le lighting 
    // TODO PHONG model for textures

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentshaderGLSL, NULL);
    glCompileShader(fragmentShader);


    /* CHECK SHADER COMPILE ERROR */

    GLint isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        // Exit with failure.
        glDeleteShader(fragmentShader); // Don't leak the shader.
        printf("FragmentShader Error\n");
        exit(2);
    }

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        // Exit with failure.
        glDeleteShader(vertexShader); // Don't leak the shader.
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

unsigned int Shader_texture::getShaderProgramID()
{
    return shaderProgram;
}

Shader_texture::~Shader_texture()
{
}