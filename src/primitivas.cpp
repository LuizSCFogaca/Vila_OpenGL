#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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

/*
 * inicializaPlano:
  - Para que serve: Cria a geometria do chão/terreno da vila e configura seus buffers na GPU.
  - O que faz:
    1. Gera e vincula um Vertex Array Object (VaoPlano) para armazenar o estado dos atributos.
    2. Envia as coordenadas dos vértices (2 triângulos formando um quadrado no plano XZ, com Y=0) via VBO para o atributo 0.
    3. Envia as coordenadas UV de textura para o atributo 1, com valores de 0.0 a 16.0 para repetir (tiling) a textura de grama 16 vezes ao longo do terreno.
 */
void inicializaPlano() {
    glGenVertexArrays(1, &VaoPlano);
    glBindVertexArray(VaoPlano);

    // VBO dos vértices do plano
    float points[] = {
        // chão triângulo 1
         0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f,  0.75f,
        -0.75f, 0.0f, -0.75f,
        // chão triângulo 2
        -0.75f, 0.0f, -0.75f,
         0.75f, 0.0f, -0.75f,
         0.75f, 0.0f,  0.75f,
    };
    
    GLuint pvbo;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // VBO de Coordenadas de Textura (UV) - repetindo 16x pelo chão
    float uvs[] = {
        16.0f,  0.0f,
         0.0f,  0.0f,
         0.0f, 16.0f,

         0.0f, 16.0f,
        16.0f, 16.0f,
        16.0f,  0.0f,
    };
    
    GLuint uvbo;
    glGenBuffers(1, &uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

/*
 * inicializaCubo:
  - Para que serve: Cria a malha de um cubo unitário (centrado na origem, de -0.5 a +0.5 em cada eixo)
  - O que faz:
    1. Gera e vincula o VaoCubo.
    2. Define 36 vértices (6 faces * 2 triângulos por face * 3 vértices) e envia ao VBO de posições (atributo 0).
    3. Define as coordenadas UV para cada uma das faces (atributo 1), mapeando a textura de [0,0] a [1,1] em cada face individualmente.
*/
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

    // VBO das Coordenadas de Textura (UV) para cada uma das 6 faces (2 triângulos cada)
    float uvs[] = {
        // face frontal
        1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f,
        // face traseira
        1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  1.0f, 1.0f,  0.0f, 0.0f,
        // face esquerda
        1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
        0.0f, 1.0f,  0.0f, 0.0f,  1.0f, 0.0f,
        // face direita
        0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f,
        // face baixo
        0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,
        // face cima
        0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,
    };
    
    GLuint uvbo;
    glGenBuffers(1, &uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

/*
  inicializaPiramide:
  - Para que serve: Constrói uma pirâmide de base quadrada
  - O que faz:
    1. Gera e vincula o VaoPiramide.
    2. Define 18 vértices (2 triângulos na base quadrada em Y=0 + 4 triângulos inclinados que convergem para o topo em (0, 1, 0)) e envia ao VBO de posições (atributo 0).
    3. Envia as coordenadas UV correspondentes (atributo 1), mapeando a textura nas faces triangulares.

 */
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

    // VBO de Coordenadas de Textura (UV) - 18 vértices (mapeamento idêntico nas 4 faces para mesma aparência)
    float uvs[] = {
        // Base quadrada (2 triângulos)
        1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,
        0.0f, 1.0f,  1.0f, 1.0f,  1.0f, 0.0f,
        // Parede frontal (triângulo de fora 1)
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        // Parede direita (triângulo de fora 2)
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        // Parede traseira (triângulo de fora 3)
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f,
        // Parede esquerda (triângulo de fora 4)
        0.0f, 0.0f,  1.0f, 0.0f,  0.5f, 1.0f
    };
    
    GLuint uvbo;
    glGenBuffers(1, &uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

/*
  inicializaCilindro:
  - Para que serve: Gera proceduralmente a malha de um cilindro 3D (com bases circulares fechadas e corpo lateral
  - O que faz:
    1. Circunferência em fatias (20 fatias angulares de 2*PI/20 radianos) usando funções trigonométricas (seno e cosseno) para calcular os pontos de raio 0.75.
    2. Para cada fatia angular, gera:
       - Triângulo da tampa superior (ápice central em Y = altura).
       - Triângulo da tampa inferior (ápice central em Y = 0).
       - Dois triângulos formando a face lateral (quadrilátero retangular).
    3. Preenche vetores de coordenadas de vértices e cores sintéticas (para sombreamento de topo/fundo/lateral), configurando os respectivos VBOs nos atributos 0 e 1 do VaoCilindro.
    4. Atualiza a contagem total de vértices (NumVerticesCilindro) para uso posterior na chamada glDrawArrays.
 
 */
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

/*
  carregaOBJ:
  - Para que serve: Lê e processa um arquivo de modelo 3D externo no formato Wavefront (.obj)
  - O que faz:
    1. Abre o arquivo em disco e faz a análise léxica linha a linha:
       - 'v': posições de vértices no espaço 3D (vec3).
       - 'vt': coordenadas de textura UV (vec2).
       - 'vn': normais de iluminação (vec3).
       - 'f': índices que formam as faces (suporta triângulos, quadriláteros e polígonos via triangulação em leque / Fan Triangulation).
    2. Desindexa os dados, montando um buffer linear contínuo (layout intercalado de 8 floats por vértice: 3 posição + 2 UV + 3 normal).
    3. Envia o buffer para a GPU em um VBO único e configura os ponteiros de atributos (loc 0: posições, loc 1: UVs, loc 2: normais) dentro de um novo VAO retornado pela função.
    4. Define por referência o total de vértices (nVertices) gerados.
 */
GLuint carregaOBJ(const char* caminho, int& nVertices) {
    std::vector<glm::vec3> posicoes, normais;
    std::vector<glm::vec2> uvs;
    std::vector<GLfloat> buffer;

    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "Erro ao abrir OBJ: " << caminho << std::endl;
        nVertices = 0;
        return 0;
    }

    std::string linha;
    while (std::getline(arq, linha)) {
        std::istringstream ss(linha);
        std::string tipo;
        ss >> tipo;
        if (tipo == "v") {
            glm::vec3 v; ss >> v.x >> v.y >> v.z; posicoes.push_back(v);
        } else if (tipo == "vt") {
            glm::vec2 t; ss >> t.x >> t.y; uvs.push_back(t);
        } else if (tipo == "vn") {
            glm::vec3 n; ss >> n.x >> n.y >> n.z; normais.push_back(n);
        } else if (tipo == "f") {
            struct VerticeFace { int vi = 0, ti = 0, ni = 0; };
            std::vector<VerticeFace> faceVertices;
            std::string token;
            while (ss >> token) {
                // formatos: v | v/vt | v//vn | v/vt/vn (indices comecam em 1)
                VerticeFace vf;
                std::istringstream ts(token);
                std::string idx;
                if (std::getline(ts, idx, '/') && !idx.empty()) vf.vi = std::stoi(idx);
                if (std::getline(ts, idx, '/') && !idx.empty()) vf.ti = std::stoi(idx);
                if (std::getline(ts, idx)) vf.ni = std::stoi(idx);
                faceVertices.push_back(vf);
            }

            // Triangulacao em leque (Fan Triangulation) para suportar 3, 4 ou mais vertices por face
            for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                VerticeFace tri[3] = { faceVertices[0], faceVertices[i], faceVertices[i + 1] };
                for (int j = 0; j < 3; ++j) {
                    glm::vec3 p = (tri[j].vi > 0 && tri[j].vi <= (int)posicoes.size()) ? posicoes[tri[j].vi - 1] : glm::vec3(0.0f);
                    glm::vec2 t = (tri[j].ti > 0 && tri[j].ti <= (int)uvs.size()) ? uvs[tri[j].ti - 1] : glm::vec2(0.0f);
                    glm::vec3 n = (tri[j].ni > 0 && tri[j].ni <= (int)normais.size()) ? normais[tri[j].ni - 1] : glm::vec3(0.0f, 1.0f, 0.0f);
                    buffer.insert(buffer.end(), {p.x, p.y, p.z, t.x, t.y, n.x, n.y, n.z});
                }
            }
        }
    }

    nVertices = (int)(buffer.size() / 8);

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(GLfloat), buffer.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLsizei stride = 8 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return vao;
}

/*
  carregaTextura:
  - Para que serve: Carrega uma imagem do disco (JPG/PNG) para a memória de vídeo e configura o objeto de textura 2D do OpenGL.
  - O que faz:
    1. Utiliza a biblioteca stb_image com inversão vertical habilitada (stbi_set_flip_vertically_on_load) para alinhar a origem da imagem com o padrão UV do OpenGL (origem no canto inferior esquerdo).
    2. Identifica o formato de canais da imagem (tons de cinza: GL_RED, RGB ou RGBA).
    3. Envia os pixels para a GPU através de glTexImage2D e gera a cadeia de mipmaps (glGenerateMipmap) para filtragem eficiente em diferentes distâncias.
    4. Define os parâmetros de amostragem de textura:
       - GL_TEXTURE_WRAP_S e GL_TEXTURE_WRAP_T como GL_REPEAT (permite repetir texturas no chão e superfícies).
       - GL_TEXTURE_MIN_FILTER como GL_LINEAR_MIPMAP_LINEAR (trilinear filtering para evitar serrilhado e moiré à distância).
       - GL_TEXTURE_MAG_FILTER como GL_LINEAR (interpolação bilinear suave quando o objeto está próximo).
    5. Libera a memória RAM da imagem com stbi_image_free e retorna o identificador (textureID) gerado pelo OpenGL.
 */
GLuint carregaTextura(const char* caminho) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int largura, altura, numCanais;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* dados = stbi_load(caminho, &largura, &altura, &numCanais, 0);

    if (dados) {
        GLenum formato = GL_RGB;
        if (numCanais == 1) formato = GL_RED;
        else if (numCanais == 3) formato = GL_RGB;
        else if (numCanais == 4) formato = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, dados);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Repeticao e filtros lineares para visual suave
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(dados);
        std::cout << "[Textura] Carregada com sucesso: " << caminho << " (" << largura << "x" << altura << ")" << std::endl;
    } else {
        std::cerr << "[Textura] Falha ao carregar textura: " << caminho << std::endl;
        stbi_image_free(dados);
    }

    return textureID;
}
