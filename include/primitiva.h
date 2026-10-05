#pragma once
#include <glad/glad.h>

extern GLuint VaoPlano;
extern GLuint VaoCubo;
extern GLuint VaoPiramide;
extern GLuint VaoCilindro;
extern int NumVerticesCilindro;

void inicializaPlano();
void inicializaCubo();
void inicializaPiramide();
void inicializaCilindro();

// Carrega um .obj triangulado. Layout por vertice: pos(3) uv(2) normal(3)
GLuint carregaOBJ(const char* caminho, int& nVertices);

// Carrega textura 2D usando stb_image
GLuint carregaTextura(const char* caminho);
