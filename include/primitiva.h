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