#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Shader_Compilation
{
public:
	Shader_Compilation();
	~Shader_Compilation();
    unsigned int getShaderProgramID();

private:
    unsigned int shaderProgram = NULL;
};

Shader_Compilation::Shader_Compilation()
{


    /*
    GLSL vertex shaderand fragment shader
    */

    //vertexshader
    //const char* vertexshaderGLSL = "#version 450 core\nlayout(location = 0) in vec3 aPos;\nvoid main()\n{\ngl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n}";
            const char* vertexshaderGLSL = R"glsl(
                #version 450 core

                layout(location = 0) in vec3 modelPos;
                layout(location = 1) in vec3 colorDataIn;
                layout(location = 2) in vec3 normalsDataIn;

                uniform mat4 Model;
                uniform mat4 ViewProjection;

                out vec3 vertexNormals;
                out vec3 fragPos;
                out vec3 colorsVertex;

                void main()
                {
                    gl_Position = ViewProjection * Model * vec4(modelPos, 1.0f);
                    vertexNormals = normalsDataIn;
                    fragPos = vec3(Model * vec4(modelPos, 1.0f));
                    colorsVertex = colorDataIn;
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

                out vec4 FragColor;

                uniform vec3 eyePos;
               
                struct Material {
                    float shininess;
                    float refract_indice;

                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;
                    vec3 color;
                    vec3 reflective;
                };
                uniform Material material;

                struct Light {
                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;
                    vec3 position;
                };
                uniform Light light;               
            
                void main()
                {
                    // AMBIENT

                    vec3 ambient = light.ambient * material.ambient;
                    
                    /* ******************* */                    

                    // DIFFUSE

                    vec3 norm = normalize(vertexNormals);
                    vec3 lightDir = normalize(light.position - fragPos);

                    float diffuseDot = max( 0.0f, dot( norm, lightDir ) );            
                    vec3 diffuse = light.diffuse * diffuseDot * material.diffuse;

                    /* ******************** */


                    // SPECULAR

                    vec3 eyeNormal = normalize(fragPos - eyePos);
                    vec3 I = normalize(norm - eyeNormal);
                    vec3 R = refract(I, norm, 1/material.refract_indice);

                    float eyeReflectionAngle = max(dot(eyeNormal, R), 0.0f);
                    float spec = pow(eyeReflectionAngle, material.shininess);

                    vec3 specular = light.specular * spec * material.specular;

                    /* ******************** */


                    /* REFLECTION */
                    
                    // TODO            

                    /* ******************** */

                    // result
                    vec3 result = (ambient + diffuse + specular) + (colorsVertex + material.color);
	                FragColor = vec4(result, 1.0f);
                }
        )glsl";
        
    // TODO Comprendre le lighting 

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentshaderGLSL, NULL);
    glCompileShader(fragmentShader);


    /* CHECK SHADER COMPILE ERROR */

    GLint isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);

        // Exit with failure.
        glDeleteShader(fragmentShader); // Don't leak the shader.
        printf("FragmentShader Error\n");
        exit(2);
    }

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);

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

unsigned int Shader_Compilation::getShaderProgramID()
{
    return shaderProgram;
}

Shader_Compilation::~Shader_Compilation()
{
}