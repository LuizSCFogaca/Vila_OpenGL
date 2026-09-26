#include <iostream>
#include "primitiva.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint VaoPlano = 0;
GLuint VaoCubo = 0;
GLuint VaoPiramide = 0;
GLuint VaoCilindro = 0;
int NumVerticesCilindro = 0;

void inicializaPlano() {
    glGenVertexArrays(1, &VaoPlano);
    glBindVertexArray(VaoPlano);

    // VBO dos vértices do cubo
    float points[] = {
        //chão triangulo 1
        0.75f, 0.0f, 0.75f,
        -0.75f, 0.0f, 0.75f,
        -0.75f, 0.0f, -0.75f,
        //chão triangulo 2
         -0.75f, 0.0f, -0.75f,
         0.75f, 0.0f,-0.75f,
         0.75f, 0.0f, 0.75f,
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores
    float cores[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void inicializaCubo(){
    glGenVertexArrays(1, &VaoCubo);
    glBindVertexArray(VaoCubo);

    // VBO dos vértices do cubo
    float points[] = {
        // face frontal
        0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // face traseira
        0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
       -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // face esquerda
       -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,
        // face direita
        0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
        // face baixo
       -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,
        // face cima
       -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores
    float cores[] = {
        // face frontal - vermelha
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // face traseira - verde
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // face esquerda - azul
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // face direita - ciano
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // face baixo - magenta
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // face cima - amarelo
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

}

void inicializaPiramide(){
    glGenVertexArrays(1, &VaoPiramide);
    glBindVertexArray(VaoPiramide);

    // 18 vértices (Base + 4 paredes triangulares)
    float points[] = {
        // Base quadrada (2 triângulos, Y = 0.0f)
         0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f, -0.75f,

        -0.75f, 0.0f, -0.75f,
         0.75f, 0.0f, -0.75f,
         0.75f, 0.0f,  0.75f,

        // Parede frontal (Z = +0.75)
        -0.75f, 0.0f,  0.75f,
         0.75f, 0.0f,  0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede direita (X = +0.75)
         0.75f, 0.0f,  0.75f,
         0.75f, 0.0f, -0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede traseira (Z = -0.75)
         0.75f, 0.0f, -0.75f,
        -0.75f, 0.0f, -0.75f,
         0.0f,  1.0f,  0.0f,

        // Parede esquerda (X = -0.75)
        -0.75f, 0.0f, -0.75f,
        -0.75f, 0.0f,  0.75f,
         0.0f,  1.0f,  0.0f
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO das cores (18 vértices com cores distintas por face)
    float cores[] = {
        // Base (cinza escuro)
        0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,
        0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,  0.3f, 0.3f, 0.3f,
        // Parede frontal (vermelho telha)
        0.9f, 0.2f, 0.2f,  0.9f, 0.2f, 0.2f,  0.9f, 0.2f, 0.2f,
        // Parede direita (laranja telha)
        0.9f, 0.5f, 0.2f,  0.9f, 0.5f, 0.2f,  0.9f, 0.5f, 0.2f,
        // Parede traseira (vermelho escuro)
        0.7f, 0.1f, 0.1f,  0.7f, 0.1f, 0.1f,  0.7f, 0.1f, 0.1f,
        // Parede esquerda (marrom claro)
        0.8f, 0.3f, 0.2f,  0.8f, 0.3f, 0.2f,  0.8f, 0.3f, 0.2f
    };
    
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void inicializaCilindro() {
    glGenVertexArrays(1, &VaoCilindro);
    glBindVertexArray(VaoCilindro);

    const int segmentos = 20;
    const float raio = 0.75f;
    const float altura = 1.0f;
    const float pi = 3.14159265359f;
    const float passo = 2.0f * pi / (float)segmentos;

    std::vector<float> points;
    std::vector<float> cores;

    for (int i = 0; i < segmentos; i++) {
        float ang1 = (float)i * passo;
        float ang2 = (float)(i + 1) * passo;

        float x1 = raio * cos(ang1);
        float z1 = raio * sin(ang1);
        float x2 = raio * cos(ang2);
        float z2 = raio * sin(ang2);

        // 1. Tampa superior (Y = altura)
        points.push_back(0.0f); points.push_back(altura); points.push_back(0.0f);
        points.push_back(x1);   points.push_back(altura); points.push_back(z1);
        points.push_back(x2);   points.push_back(altura); points.push_back(z2);

        for (int k = 0; k < 3; k++) {
            cores.push_back(0.7f); cores.push_back(0.7f); cores.push_back(0.7f);
        }

        // 2. Tampa inferior (Y = 0.0f)
        points.push_back(0.0f); points.push_back(0.0f);   points.push_back(0.0f);
        points.push_back(x2);   points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x1);   points.push_back(0.0f);   points.push_back(z1);

        for (int k = 0; k < 3; k++) {
            cores.push_back(0.4f); cores.push_back(0.4f); cores.push_back(0.4f);
        }

        // 3. Lateral do cilindro (2 triângulos)
        // Triângulo 1
        points.push_back(x1); points.push_back(0.0f);   points.push_back(z1);
        points.push_back(x2); points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x1); points.push_back(altura); points.push_back(z1);

        // Triângulo 2
        points.push_back(x2); points.push_back(0.0f);   points.push_back(z2);
        points.push_back(x2); points.push_back(altura); points.push_back(z2);
        points.push_back(x1); points.push_back(altura); points.push_back(z1);

        // Cor lateral
        for (int k = 0; k < 6; k++) {
            cores.push_back(0.2f); cores.push_back(0.5f); cores.push_back(0.8f);
        }
    }

    NumVerticesCilindro = (int)(points.size() / 3);

    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, cores.size() * sizeof(float), cores.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}