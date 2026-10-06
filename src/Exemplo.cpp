// Câmera

// Neste exemplo, especificamos uma câmera virtual através da aplicação de transformações de projeção e lookAt
#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "primitiva.h"

GLFWwindow* Window = nullptr;
GLuint Shader_programm = 0;
GLuint Vao = 0;
GLuint VaoCerca = 0;
int NumVerticesCerca = 0;
GLuint TexGrama = 0;
GLuint TexFolhas = 0;

int WIDTH = 800;
int HEIGHT = 600;

float Tempo_entre_frames = 0.0f; // variavel utilizada para movimentar a camera

// Variáveis referentes a câmera virtual e sua projeção
float Cam_speed = 5.0f; // velocidade da camera aumentada um pouco para navegação livre
glm::vec3 Cam_pos = glm::vec3(0.0f, 0.0f, 2.0f); // posicao inicial da câmera
glm::vec3 Cam_front = glm::vec3(0.0f, 0.0f, -1.0f); // vetor para onde a câmera está olhando
glm::vec3 Cam_up = glm::vec3(0.0f, 1.0f, 0.0f); // vetor "para cima" global

float Cam_yaw = 0.0f; // ângulo de rotação da câmera (esquerda/direita)
float Cam_pitch = 0.0f; // ângulo de inclinação da câmera (cima/baixo)
float Cam_fov = 67.0f;

// Variáveis de controle do mouse
double lastX = WIDTH / 2.0;
double lastY = HEIGHT / 2.0;
bool primeiro_mouse = true;

// Estado da fogueira (Interação do usuário)
bool fogueiraAcesa = true;
bool teclaF_pressionada = false;

/*
  redimensionaCallback:
  - Para que serve: Ajusta a área de desenho do OpenGL sempre que a janela da aplicação for redimensionada pelo usuário.
  - O que faz:
    1. Atualiza as variáveis globais WIDTH e HEIGHT com as novas dimensões em pixels.
    2. Chama glViewport(0, 0, w, h) para sincronizar o espaço de tela com a nova resolução, evitando distorção na projeção da cena.
 */
void redimensionaCallback(GLFWwindow* window, int w, int h) {
    WIDTH = w;
    HEIGHT = h;
    glViewport(0, 0, WIDTH, HEIGHT);
}

/*
  mouse_callback:
  - Para que serve: Controla a câmera usando o movimento do mouse.
  - O que faz:
    1. Trata a primeira leitura do cursor (primeiro_mouse) para evitar saltos bruscos na rotação inicial.
    2. Calcula o delta de deslocamento (xoffset e yoffset) do cursor entre o frame anterior e o atual.
    3. Aplica uma sensibilidade e atualiza os ângulos de Euler da câmera:
       - Cam_yaw: rotação horizontal (olhar para esquerda/direita).
       - Cam_pitch: rotação vertical (olhar para cima/baixo).
    4. Aplica uma trava no pitch entre -89.0 e +89.0 graus para impedir o efeito de gimbal lock ou inversã.
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (primeiro_mouse) {
        lastX = xpos;
        lastY = ypos;
        primeiro_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido, pois as coordenadas Y vão de cima para baixo
    
    lastX = xpos;
    lastY = ypos;

    float sensibilidade = 0.1f;
    xoffset *= sensibilidade;
    yoffset *= sensibilidade;

    Cam_yaw -= xoffset;
    Cam_pitch += yoffset;

    // Trava do pitch para evitar que a câmera dê uma cambalhota
    if (Cam_pitch > 89.0f) Cam_pitch = 89.0f;
    if (Cam_pitch < -89.0f) Cam_pitch = -89.0f;
}

/*
  inicializaOpenGL:
  - Para que serve: Inicializa a biblioteca de janelas e eventos (GLFW), cria a janela gráfica e carrega os ponteiros das funções do OpenGL via GLAD.
  - O que faz:
    1. Inicializa o GLFW com glfwInit().
    2. Cria uma janela com resolução WIDTH x HEIGHT e título especificado.
    3. Registra os callbacks de redimensionamento e de posição do cursor do mouse.
    4. Oculta e trava o cursor dentro da janela (GLFW_CURSOR_DISABLED) para navegação fluida em 3D.
    5. Torna o contexto OpenGL atual com glfwMakeContextCurrent(Window).
    6. Carrega os ponteiros das extensões do OpenGL com gladLoadGLLoader.
 */
void inicializaOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    Window = glfwCreateWindow(WIDTH, HEIGHT, "Exemplo - Camera Livre com Mouse", NULL, NULL);
    if (!Window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(Window, redimensionaCallback);
    
    // Registra a função do mouse e oculta o cursor na tela
    glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar o GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/*
  desenhaCasa:
  - Para que serve: Renderiza uma casa completa no cenário, combinando primitivas básicas.
  - O que faz:
    1. Monta uma matriz base (matBase) com a translação para 'posicao', rotação em Y ('anguloRotacao') e escala geral ('escala').
    2. Desenha as paredes da casa: parte da matBase, eleva 0.5 em Y (para assentar o cubo exatamente sobre o plano do chão Y=0), envia a cor bege ao shader e desenha o VaoCubo.
    3. Desenha o telhado: desloca para o topo da parede (Y=1.0), escala para criar beirais, envia cor de telha cerâmica e desenha a VaoPiramide.
    4. Desenha a porta: posiciona na face frontal (Z=0.51 para evitar z-fighting com a parede), achata em Z, envia cor marrom de madeira e desenha o VaoCubo.
 
 */
void desenhaCasa(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
    // Matriz base da casa (aplica a posição, rotação e escala global da casa)
    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    glm::mat4 matCorpo = matBase;
    // Eleva 0.5 no eixo Y para a base do cubo encostar no chão (Y=0)
    matCorpo = glm::translate(matCorpo, glm::vec3(0.0f, 0.5f, 0.0f));
    matCorpo = glm::scale(matCorpo, glm::vec3(1.0f, 1.0f, 1.0f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCorpo));
    glUniform3f(corLoc, 0.85f, 0.80f, 0.65f); // Cor bege/areia para as paredes
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glm::mat4 matTelhado = matBase;
    // Posiciona na altura do topo do cubo (Y=1.0)
    matTelhado = glm::translate(matTelhado, glm::vec3(0.0f, 1.0f, 0.0f));
    // Escala 0.8 para criar um beiral suave (0.8 * 1.5 = 1.2 de largura)
    matTelhado = glm::scale(matTelhado, glm::vec3(0.8f, 0.6f, 0.8f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTelhado));
    glUniform3f(corLoc, 0.80f, 0.25f, 0.15f); // Cor vermelho telha
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glm::mat4 matPorta = matBase;
    matPorta = glm::translate(matPorta, glm::vec3(0.0f, 0.25f, 0.51f));
    matPorta = glm::scale(matPorta, glm::vec3(0.25f, 0.5f, 0.05f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPorta));
    glUniform3f(corLoc, 0.35f, 0.18f, 0.05f); // Cor madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

/*
  desenhaArvoreCubo:
  - Para que serve: Renderiza uma árvore estilizada (estilo arbusto cúbico) composta por tronco cilíndrico e copa cúbica texturizada.
  - O que faz:
    1. Define a matriz Model base a partir da posição, rotação e escala passadas.
    2. Desenha o tronco: escala o cilindro procedural em X/Z (para torná-lo esguio) e estica em Y com cor marrom, renderizando o VaoCilindro.
    3. Desenha a copa: translada para acima do tronco, escala um cubo volumoso, ativa a unidade de textura 0 (TexFolhas), ativa o uniform usaTextura = 1 e desenha o VaoCubo.
    4. Desativa o uso de textura ao final para não interferir nas próximas renderizações.
 
 */
void desenhaArvoreCubo(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)){
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f,1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    // tronco (Cilindro de Madeira)
    glm::mat4 matTronco = matBase;
    float alturaTronco = 1.2f;
    matTronco = glm::scale(matTronco, glm::vec3(0.25f, alturaTronco, 0.25f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTronco));
    glUniform3f(corLoc, 0.40f, 0.22f, 0.10f); // Marrom casca de árvore
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
        // topo(Cubo Verde de Folhas) - com Textura
    glm::mat4 matCopa1 = matBase;
    matCopa1 = glm::translate(matCopa1, glm::vec3(0.0f, alturaTronco + 0.5f, 0.0f));
    matCopa1 = glm::scale(matCopa1, glm::vec3(1.4f, 1.5f, 1.4f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matCopa1));
    glUniform3f(corLoc, 1.0f, 1.0f, 1.0f); // Cor neutra para manter a cor natural da textura
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexFolhas);
    glUniform1i(glGetUniformLocation(Shader_programm, "tex"), 0);

    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 0);
}

/*
  desenhaArvorePiramide:
  - Para que serve: Renderiza um pinheiro/conífera combinando um tronco cilíndrico com copa cônica/piramidal texturizada.
  - O que faz:
    1. Configura a matriz base com posição, ângulo e escala.
    2. Renderiza o tronco cilíndrico fino e vertical com cor de madeira marrom.
    3. Desloca a copa ligeiramente acima da base e escala a pirâmide para ficar pontiaguda e alta (escala em Y = 2.0).
    4. Aplica a textura de folhas (TexFolhas) sobre as faces da pirâmide via VaoPiramide e desativa o uniform após o desenho.
 */
void desenhaArvorePiramide(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala= glm::vec3(1.0f)){
        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f,1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);

    //tronco(Cilindro de Madeira)
    glm::mat4 matTronco = matBase;
    float alturaTronco = 1.5f;
    matTronco = glm::scale(matTronco, glm::vec3(0.25f, alturaTronco, 0.25f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTronco));
    glUniform3f(corLoc, 0.40f, 0.22f, 0.10f); // Marrom casca de árvore
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);

    //topo(piramide verde) - com Textura
    glm::mat4 matFolhas1 = glm::translate(matBase, glm::vec3(0.0f, 0.6f, 0.0f));
    matFolhas1 = glm::scale(matFolhas1, glm::vec3(1.0f, 2.0f, 1.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFolhas1));
    glUniform3f(corLoc, 1.0f, 1.0f, 1.0f); // Cor neutra para manter a cor natural da textura
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexFolhas);
    glUniform1i(glGetUniformLocation(Shader_programm, "tex"), 0);

    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 0);
}

/*
  desenhaMoinho:
  - Para que serve: Renderiza um moinho de vento completo com animação contínua e autônoma das pás giratórias, cumprindo o requisito de animação dinâmica atrelada ao tempo.
  - O que faz:
    1. Cria a base espacial (matBase) com posicionamento, orientação e escala.
    2. Desenha a torre cilíndrica de alvenaria escalonada em altura (Y = 3.2).
    3. Posiciona o telhado piramidal no topo da torre.
    4. Cria o eixo/rotor frontal projetado para a frente da fachada (Z = 0.8).
    5. Animação contínua: obtém o tempo decorrido com glfwGetTime(), calcula o ângulo de rotação contínua (anguloGiro = tempo * 50.0 graus/s) e aplica a rotação no eixo Z local.
    6. Desenha 4 pás dispostas em cruz (rotações incrementais de 90 graus), cada uma com translação radial e escala retangular fina.
  */
void desenhaMoinho(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala=glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
    // basw(Posiciona e orienta o moinho no mundo)
    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);
    
    // torre (Cilindro Branco/Cinza Alto)
    float alturaTorre = 3.2f;
    glm::mat4 matTorre = matBase;
    // O cilindro original tem raio 0.75. Multiplicado por 1.0 fica com raio 0.75 e altura 3.2
    matTorre = glm::scale(matTorre, glm::vec3(1.0f, alturaTorre, 1.0f));
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTorre));
    glUniform3f(corLoc, 0.88f, 0.85f, 0.80f); // Cor pedra/reboco claro
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
    // teto (Pirâmide no Topo da Torre)
    glm::mat4 matTeto = matBase;
    matTeto = glm::translate(matTeto, glm::vec3(0.0f, alturaTorre, 0.0f));
    matTeto = glm::scale(matTeto, glm::vec3(1.2f, 1.0f, 1.2f)); // Beiral ligeiramente maior
    
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTeto));
    glUniform3f(corLoc, 0.55f, 0.25f, 0.15f); // Telha marrom/vermelha
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    
    // eixo (Ponto Central de Rotação das Pás)
    // Colocamos o eixo próximo ao topo da torre (Y = 2.6) e projetado para a frente (Z = 0.8)
    glm::mat4 matEixo = matBase;
    matEixo = glm::translate(matEixo, glm::vec3(0.0f, 2.6f, 0.8f));
    
    // Hub Central (Cilindro ou Cubo pequeno no miolo das pás)
    glm::mat4 matMiolo = glm::scale(matEixo, glm::vec3(0.2f, 0.2f, 0.2f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matMiolo));
    glUniform3f(corLoc, 0.25f, 0.15f, 0.05f); // Madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    
    // 5. ANIMAÇÃO DAS PÁS (Giro contínuo no eixo Z)
    float anguloGiro = (float)glfwGetTime() * 50.0f;
    
    // Aplica a rotação contínua no eixo Z local do rotor
    glm::mat4 matRotorAnimado = glm::rotate(matEixo, glm::radians(anguloGiro), glm::vec3(0.0f,.0f, 1.0f));
    
    // Desenha as 4 pás espaçadas de 90 em 90 graus
    for (int i = 0; i < 4; i++) {
        glm::mat4 matPa = matRotorAnimado;
        // Gira cada uma das 4 pás na sua respectiva orientação (0°, 90°, 180°, 270°)
        matPa = glm::rotate(matPa, glm::radians(i * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        
        // Desloca o centro da pá para frente no eixo Y para ela se estender para fora a partirdo centro
        matPa = glm::translate(matPa, glm::vec3(0.0f, 0.9f, 0.0f));
            
        // Estica a pá: fina em X e Z, comprida em Y
        matPa = glm::scale(matPa, glm::vec3(0.18f, 1.5f, 0.03f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPa));
            
        // Alterna tons sutis ou use uma cor de madeira/tecido nas pás
        glUniform3f(corLoc, 0.75f, 0.70f, 0.55f); // Tecido/Madeira clara
        glBindVertexArray(VaoCubo);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

/*
  desenhaFogueira:
  - Para que serve: Renderiza a fogueira central da vila com comportamento dual animado e interativo (ligada/desligada via teclado).
  - O que faz:
    1. Renderiza a base de brasas cilíndrica (com cor variando entre brasa incandescente e carvão frio dependendo de 'fogueiraAcesa').
    2. Renderiza o círculo protetor de 8 pedras distribuídas trigonometricamente com ângulos regulares (360/8 graus) e leves variações de tamanho.
    3. Posiciona 4 toras de lenha cilíndricas inclinadas em 35 graus em direção ao centro da fogueira.
    4. Estado Aceso (fogueiraAcesa == true):
       - Anima chamas com oscilações sinusoidais assíncronas (via seno e cosseno do tempo) em duas camadas piramidais: chama externa alaranjada e interna amarela em rotação.
    5. Estado Apagado (fogueiraAcesa == false):
       - Simula partículas procedurais de fumaça subindo ciclicamente (usando fmod com tempo), com dispersão horizontal e aumento gradual de tamanho à medida que se dissipam no ar.
 */
void desenhaFogueira(glm::vec3 posicao, float escalaGeral = 1.0f) {
        GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
        GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");
    
        // 1. Matriz Base da Fogueira
        glm::mat4 matBase = glm::mat4(1.0f);
        matBase = glm::translate(matBase, posicao);
        matBase = glm::scale(matBase, glm::vec3(escalaGeral));
    
        // ==========================================
        // 2. BRASAS / CINZAS (Base no Chão)
        // ==========================================
        glm::mat4 matBrasa = matBase;
        matBrasa = glm::scale(matBrasa, glm::vec3(0.6f, 0.05f, 0.6f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matBrasa));
        if (fogueiraAcesa) {
            glUniform3f(corLoc, 0.35f, 0.12f, 0.05f); // Carvão avermelhado / em brasa
        } else {
            glUniform3f(corLoc, 0.12f, 0.10f, 0.10f); // Carvão frio / apagado
        }
        glBindVertexArray(VaoCilindro);
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
    
        // ==========================================
        // 3. CÍRCULO DE PEDRAS AO REDOR
        // ==========================================
        int numPedras = 8;
        float raioCirculo = 0.55f;
        for (int i = 0; i < numPedras; i++) {
            float angulo = glm::radians((float)i * (360.0f / numPedras));
            float px = cos(angulo) * raioCirculo;
            float pz = sin(angulo) * raioCirculo;
    
            glm::mat4 matPedra = matBase;
            matPedra = glm::translate(matPedra, glm::vec3(px, 0.06f, pz));
            // Alterna tamanhos para parecer pedras naturais
            float varTamanho = (i % 2 == 0) ? 0.14f : 0.11f;
            matPedra = glm::scale(matPedra, glm::vec3(varTamanho, 0.12f, varTamanho));
    
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPedra));
            glUniform3f(corLoc, 0.45f, 0.45f, 0.45f); // Cinza pedra
            glBindVertexArray(VaoCubo);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    
        // ==========================================
        // 4. TORAS DE LENHA INCLINADAS
        // ==========================================
        int numToras = 4;
        for (int i = 0; i < numToras; i++) {
            glm::mat4 matTora = matBase;
            // Gira cada tora para uma direção
            matTora = glm::rotate(matTora, glm::radians((float)i * 90.0f + 20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // Inclina a tora em direção ao centro
            matTora = glm::rotate(matTora, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            // Afina o cilindro e estica o comprimento
            matTora = glm::scale(matTora, glm::vec3(0.08f, 0.65f, 0.08f));
    
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTora));
            glUniform3f(corLoc, 0.30f, 0.15f, 0.05f); // Madeira escura
            glBindVertexArray(VaoCilindro);
            glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
        }
    
        if (fogueiraAcesa) {
            // ==========================================
            // 5. CHAMAS DE FOGO (Animadas com Pulsação!)
            // ==========================================
            float tempo = (float)glfwGetTime();
            // Varia suavemente entre 0.85 e 1.15 usando a função seno
            float pulsacao1 = 0.95f + 0.15f * sin(tempo * 9.0f);
            float pulsacao2 = 0.90f + 0.15f * cos(tempo * 12.0f);
        
            // Chama Externa (Maior - Vermelho/Laranja)
            glm::mat4 matFogoExterno = matBase;
            matFogoExterno = glm::translate(matFogoExterno, glm::vec3(0.0f, 0.05f, 0.0f));
            matFogoExterno = glm::scale(matFogoExterno, glm::vec3(0.40f * pulsacao1, 0.70f * pulsacao1, 0.40f * pulsacao1));
        
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFogoExterno));
            glUniform3f(corLoc, 0.95f, 0.30f, 0.05f); // Laranja avermelhado
            glBindVertexArray(VaoPiramide);
            glDrawArrays(GL_TRIANGLES, 0, 18);

            // Chama Interna (Menor e mais rápida - Amarelo brilhante)
            glm::mat4 matFogoInterno = matBase;
            matFogoInterno = glm::translate(matFogoInterno, glm::vec3(0.0f, 0.08f, 0.0f));
            matFogoInterno = glm::rotate(matFogoInterno, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            matFogoInterno = glm::scale(matFogoInterno, glm::vec3(0.25f * pulsacao2, 0.50f * pulsacao2, 0.25f * pulsacao2));

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFogoInterno));
            glUniform3f(corLoc, 1.0f, 0.85f, 0.10f); // Amarelo fogo
            glBindVertexArray(VaoPiramide);
            glDrawArrays(GL_TRIANGLES, 0, 18);
        } else {
            // ==========================================
            // 6. EFEITO DE FUMAÇA SUBINDO (Quando Apagada)
            // ==========================================
            float tempo = (float)glfwGetTime();
            int numParticulas = 6;
            float alturaMax = 2.5f;

            for (int i = 0; i < numParticulas; i++) {
                // Cada partícula tem um deslocamento de fase vertical
                float offset = (float)i * (alturaMax / (float)numParticulas);
                // Altura subindo ciclicamente através de fmod
                float progresso = fmod(tempo * 0.8f + offset, alturaMax);
                float normalizado = progresso / alturaMax; // 0.0 (base) a 1.0 (topo)

                // Vento / oscilação suave nos eixos X e Z
                float ventoX = sin(tempo * 1.5f + (float)i * 1.2f) * 0.12f * (1.0f + normalizado * 2.0f);
                float ventoZ = cos(tempo * 1.3f + (float)i * 0.9f) * 0.12f * (1.0f + normalizado * 2.0f);

                // Escala cresce à medida que a fumaça se dissipa
                float escalaFumaca = 0.12f + normalizado * 0.35f;

                glm::mat4 matFumaca = matBase;
                matFumaca = glm::translate(matFumaca, glm::vec3(ventoX, 0.2f + progresso, ventoZ));
                matFumaca = glm::rotate(matFumaca, glm::radians(tempo * 35.0f + (float)i * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                matFumaca = glm::scale(matFumaca, glm::vec3(escalaFumaca, escalaFumaca * 0.8f, escalaFumaca));

                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matFumaca));
                
                // Cor vai clareando gradualmente com a altitude
                float tomFumaca = 0.50f + normalizado * 0.35f;
                glUniform3f(corLoc, tomFumaca, tomFumaca, tomFumaca + 0.04f);

                if (i % 2 == 0) {
                    glBindVertexArray(VaoCubo);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                } else {
                    glBindVertexArray(VaoCilindro);
                    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
                }
            }
        }
}

/*
  desenharPoco:
  - Para que serve: Renderiza um poço d'água medieval detalhado
  - O que faz:
    1. Define a matriz Model base para posicionar o poço na vila.
    2. Desenha a mureta cilíndrica de pedra cinza (VaoCilindro).
    3. Insere a água interna com um cilindro azul concêntrico ligeiramente mais estreito e baixo.
    4. Adiciona dois pilares verticais de madeira (cubos estreitos) nas laterais esquerda e direita.
    5. Adiciona a viga horizontal superior e o carretel/rolo central com o balde suspenso (VaoCubo).
    6. Coroa o topo com um telhado piramidal (VaoPiramide) para proteger a estrutura.
 */
void desenharPoco(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala = glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 matBase = glm::mat4(1.0f);
    matBase = glm::translate(matBase, posicao);
    matBase = glm::rotate(matBase, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    matBase = glm::scale(matBase, escala);

    // 1. Base / Parede de pedra do poço (Cilindro)
    float raioPoco = 0.75f;
    float alturaParede = 0.65f;
    glm::mat4 matParede = matBase;
    matParede = glm::scale(matParede, glm::vec3(raioPoco, alturaParede, raioPoco));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matParede));
    glUniform3f(corLoc, 0.50f, 0.52f, 0.53f); // Cinza pedra
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);

    // 2. Água no interior (Cilindro azul levemente menor e posicionado dentro do poço)
    glm::mat4 matAgua = matBase;
    matAgua = glm::translate(matAgua, glm::vec3(0.0f, 0.05f, 0.0f));
    matAgua = glm::scale(matAgua, glm::vec3(raioPoco * 0.85f, alturaParede * 0.85f, raioPoco * 0.85f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matAgua));
    glUniform3f(corLoc, 0.12f, 0.45f, 0.85f); // Azul água
    glBindVertexArray(VaoCilindro);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);

    // 3. Pilares de sustentação de madeira (2 pilares verticais laterais)
    float alturaPilar = 1.6f;
    float posXpilar = 0.55f;

    // Pilar esquerdo
    glm::mat4 matPilarEsq = matBase;
    matPilarEsq = glm::translate(matPilarEsq, glm::vec3(-posXpilar, alturaPilar / 2.0f, 0.0f));
    matPilarEsq = glm::scale(matPilarEsq, glm::vec3(0.1f, alturaPilar, 0.1f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPilarEsq));
    glUniform3f(corLoc, 0.35f, 0.20f, 0.10f); // Madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Pilar direito
    glm::mat4 matPilarDir = matBase;
    matPilarDir = glm::translate(matPilarDir, glm::vec3(posXpilar, alturaPilar / 2.0f, 0.0f));
    matPilarDir = glm::scale(matPilarDir, glm::vec3(0.1f, alturaPilar, 0.1f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPilarDir));
    glUniform3f(corLoc, 0.35f, 0.20f, 0.10f);
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 4. Trave superior / Viga horizontal conectando os pilares
    glm::mat4 matViga = matBase;
    matViga = glm::translate(matViga, glm::vec3(0.0f, alturaPilar, 0.0f));
    matViga = glm::scale(matViga, glm::vec3(posXpilar * 2.0f + 0.2f, 0.1f, 0.12f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matViga));
    glUniform3f(corLoc, 0.30f, 0.18f, 0.08f);
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 5. Rolo/Eixo central de corda
    glm::mat4 matRolo = matBase;
    matRolo = glm::translate(matRolo, glm::vec3(0.0f, alturaPilar - 0.25f, 0.0f));
    matRolo = glm::scale(matRolo, glm::vec3(0.7f, 0.08f, 0.08f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matRolo));
    glUniform3f(corLoc, 0.65f, 0.55f, 0.40f); // Corda clara
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Balde pendurado no meio
    glm::mat4 matBalde = matBase;
    matBalde = glm::translate(matBalde, glm::vec3(0.0f, alturaPilar - 0.65f, 0.0f));
    matBalde = glm::scale(matBalde, glm::vec3(0.18f, 0.22f, 0.18f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matBalde));
    glUniform3f(corLoc, 0.25f, 0.15f, 0.08f); // Madeira escura
    glBindVertexArray(VaoCubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 6. Telhadinho de cobertura do poço (Pirâmide)
    glm::mat4 matTelhado = matBase;
    matTelhado = glm::translate(matTelhado, glm::vec3(0.0f, alturaPilar + 0.05f, 0.0f));
    matTelhado = glm::scale(matTelhado, glm::vec3(1.1f, 0.55f, 1.1f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matTelhado));
    glUniform3f(corLoc, 0.60f, 0.25f, 0.15f); // Telha vermelha/marrom
    glBindVertexArray(VaoPiramide);
    glDrawArrays(GL_TRIANGLES, 0, 18);
}

/*
  desenhaCaminhoPedra:
  - Para que serve: Cria trilhas depedras interligando portas das casas até a praça da vila.
  - O que faz:
    1. Interpola linearmente (glm::mix) posições entre 'posInicio' e 'posFim' em uma quantidade dada de passos.
    2. Introduz variações pseudo-aleatórias (usando funções trigonométricas no índice 'i') em rotação, escala e desvio lateral, evitando a rigidez visual de uma linha artificial.
    3. Alterna o desenho entre cilindros e cubos achatados e varia os tons de cinza para conferir aspecto de pedras lapidadas naturalmente.
 */
void desenhaCaminhoPedra(glm::vec3 posInicio, glm::vec3 posFim, int passos = 8, float largura = 0.4f) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    if (passos <= 1) {
        glm::mat4 matPedra = glm::mat4(1.0f);
        matPedra = glm::translate(matPedra, glm::vec3(posInicio.x, 0.015f, posInicio.z));
        matPedra = glm::scale(matPedra, glm::vec3(largura, 0.03f, largura));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPedra));
        glUniform3f(corLoc, 0.50f, 0.50f, 0.50f);
        glBindVertexArray(VaoCilindro);
        glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
        return;
    }

    for (int i = 0; i <= passos; i++) {
        float t = (float)i / (float)passos;
        glm::vec3 pos = glm::mix(posInicio, posFim, t);

        // Variação orgânica na posição e formato de cada pedra do caminho
        float desvioX = sin((float)i * 1.7f) * (largura * 0.25f);
        float desvioZ = cos((float)i * 2.3f) * (largura * 0.25f);
        float tamVar = 0.85f + 0.3f * sin((float)i * 3.1f);
        float rotVar = (float)((i * 47) % 360);

        glm::mat4 matPedra = glm::mat4(1.0f);
        matPedra = glm::translate(matPedra, glm::vec3(pos.x + desvioX, 0.015f, pos.z + desvioZ));
        matPedra = glm::rotate(matPedra, glm::radians(rotVar), glm::vec3(0.0f, 1.0f, 0.0f));
        matPedra = glm::scale(matPedra, glm::vec3(largura * tamVar, 0.03f, (largura * 0.85f) * tamVar));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPedra));
        
        // Alterna tons sutis de cinza para aspecto de pedras naturais
        float tomCinza = 0.45f + 0.1f * sin((float)i * 1.5f);
        glUniform3f(corLoc, tomCinza, tomCinza, tomCinza);

        // Alterna entre cilindro achatado e cubo para dar diversidade visual
        if (i % 2 == 0) {
            glBindVertexArray(VaoCilindro);
            glDrawArrays(GL_TRIANGLES, 0, NumVerticesCilindro);
        } else {
            glBindVertexArray(VaoCubo);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

/*
  inicializaShaders:
  - Para que serve: Cria, compila e linka o programa de shaders (Shader_programm) que executa na GPU (Vertex Shader e Fragment Shader).
  - O que faz:
    1. Vertex Shader (GLSL 400):
       - Recebe posição (loc 0) e coordenadas UV (loc 1).
       - Recebe matrizes Model ('matriz'), View ('view') e Projection ('proj') como uniforms.
       - Transforma a coordenada local para espaço homogêneo de corte: gl_Position = proj * view * matriz * vec4(pos, 1.0).
       - Passa a coordenada UV interpolada para o Fragment Shader através da variável 'TexCoord'.
    2. Fragment Shader (GLSL 400):
       - Se 'usaTextura' estiver ativo, calcula a cor do fragmento multiplicando a amostragem da textura pelo tom 'corObjeto' (modulação de cor).
       - Caso contrário, desenha a cor sólida de 'corObjeto'.
    3. Checa erros de compilação e linkagem (glGetShaderiv e glGetProgramiv), emitindo logs caso ocorra falha.
 */
void inicializaShaders() {
    const char* vertex_shader = 
        "#version 400\n"
        "layout(location = 0) in vec3 vertex_posicao;\n"
        "layout(location = 1) in vec2 vertex_uv;\n"
        "uniform mat4 matriz, view, proj;\n"
        "out vec2 TexCoord;\n"
        "void main () {\n"
        "    TexCoord = vertex_uv;\n"
        "    gl_Position = proj * view * matriz * vec4(vertex_posicao, 1.0);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    
    GLint success;
    char infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        std::cerr << "Erro no vertex shader:\n" << infoLog << std::endl;
    }

    const char* fragment_shader = 
        "#version 400\n"
        "in vec2 TexCoord;\n"
        "uniform vec3 corObjeto;\n"
        "uniform sampler2D tex;\n"
        "uniform bool usaTextura;\n"
        "out vec4 frag_colour;\n"
        "void main () {\n"
        "    if (usaTextura) {\n"
        "        frag_colour = texture(tex, TexCoord) * vec4(corObjeto, 1.0);\n"
        "    } else {\n"
        "        frag_colour = vec4(corObjeto, 1.0);\n"
        "    }\n"
        "}\n";

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        std::cerr << "Erro no fragment shader:\n" << infoLog << std::endl;
    }

    Shader_programm = glCreateProgram();
    glAttachShader(Shader_programm, vs);
    glAttachShader(Shader_programm, fs);
    glLinkProgram(Shader_programm);
    
    glGetProgramiv(Shader_programm, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(Shader_programm, 512, NULL, infoLog);
        std::cerr << "Erro na linkagem do shader:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

/*
  atualizaDirecaoCamera:
  - Para que serve: Converte os ângulos de rotação esférica, determinando para onde a câmera aponta.
  - O que faz:
    1. Aplica funções trigonométricas de coordenadas esféricas para calcular os eixos cartesianos:
       - X = sin(-Yaw) * cos(Pitch)
       - Y = sin(Pitch)
       - Z = -cos(-Yaw) * cos(Pitch)
    2. Normaliza o vetor resultante para manter comprimento unitário (evitando distorções na velocidade de movimentação).
 */
void atualizaDirecaoCamera() {
    // Recalcula o vetor de direção considerando tanto o Yaw quanto o Pitch
    glm::vec3 front;
    front.x = sin(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    front.y = sin(glm::radians(Cam_pitch)); 
    front.z = -cos(glm::radians(-Cam_yaw)) * cos(glm::radians(Cam_pitch));
    Cam_front = glm::normalize(front);
}

/*
  trataTeclado:
  - Para que serve: Processa as entradas do teclado para movimentação livre no espaço 
  - O que faz:
    1. ESC: Encerra a aplicação.
    2. Calcula o vetor lateral 'Cam_right' usando o produto vetorial (cross product) entre Cam_front e Cam_up.
    3. W/S: Desloca a câmera para frente e para trás ao longo do vetor diretor da visão.
    4. A/D: Desloca lateralmente (strafe) ao longo de Cam_right.
    5. Q/E: Move estritamente na vertical no eixo Y global (subir/descer).
    6. Multiplica todo o deslocamento por 'Tempo_entre_frames' (Delta Time), garantindo que a velocidade da câmera seja independente da taxa de quadros (FPS) do computador.
    7. Z ou Botão Direito do Mouse: Aplica efeito de zoom estreitando o campo de visão (Cam_fov = 10 graus).
    8. Tecla F (Interação do Usuário): Alterna com mecanismo de debouncing o estado da fogueira entre acesa (fogo ativo) e apagada (fumaça procedural).
 */
void trataTeclado() {
    if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(Window, true);
    }

    // Calcula o vetor "Direita" da câmera usando o Cross product
    glm::vec3 Cam_right = glm::normalize(glm::cross(Cam_front, Cam_up));

    // A/D - Movimento lateral (Strafe)
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS) {
        Cam_pos -= Cam_right * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS) {
        Cam_pos += Cam_right * Cam_speed * Tempo_entre_frames;
    }

    // W/S - Movimento para frente e para trás
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS) {
        Cam_pos += Cam_front * Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS) {
        Cam_pos -= Cam_front * Cam_speed * Tempo_entre_frames;
    }

    // Subir / Descer estritamente no eixo Y global (Q e E agora fazem isso, já que o mouse cuida da rotação)
    if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS) {
        Cam_pos.y += Cam_speed * Tempo_entre_frames;
    }
    if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS) {
        Cam_pos.y -= Cam_speed * Tempo_entre_frames;
    }

    if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS || glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        Cam_fov = 10.0f;
    } else {
        Cam_fov = 67.0f;
    }

    // Tecla F: Alterna a fogueira entre acesa e apagada (com fumaça)
    if (glfwGetKey(Window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!teclaF_pressionada) {
            fogueiraAcesa = !fogueiraAcesa;
            teclaF_pressionada = true;
            std::cout << "[Interacao] Fogueira " << (fogueiraAcesa ? "ACESA (com chamas)" : "APAGADA (com fumaca saindo)") << std::endl;
        }
    } else {
        teclaF_pressionada = false;
    }
}

/*
  desenhaCerca:
  - Para que serve: Renderiza instâncias da cerca carregada a partir do modelo externo Wavefront OBJ (fence.obj).
  - O que faz:
    1. Aplica transformações afins (posição, rotação e escala) na matriz Model.
    2. Envia cor de madeira natural envelhecida para o shader.
    3. Vincula o VAO carregado do arquivo OBJ (VaoCerca) e executa o desenho de NumVerticesCerca vértices via glDrawArrays.
 */
void desenhaCerca(glm::vec3 posicao, float anguloRotacao = 0.0f, glm::vec3 escala = glm::vec3(1.0f)) {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, posicao);
    m = glm::rotate(m, glm::radians(anguloRotacao), glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::scale(m, escala);

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(m));
    glUniform3f(corLoc, 0.55f, 0.35f, 0.18f); // Madeira natural envelhecida

    glBindVertexArray(VaoCerca);
    glDrawArrays(GL_TRIANGLES, 0, NumVerticesCerca);
}

/*
  desenhaCena:
  - Para que serve: Centraliza a instanciação e disposição de todos os elementos que compõem a vila virtual.
  - O que faz:
    1. Renderiza o terreno/chão aplicando a textura repetida de grama (TexGrama) sobre o plano escalonado (10x1x10).
    2. Instancia 7 casas distribuídas radialmente em torno de uma praça central, com orientações e escalas variadas.
    3. Instancia 8 árvores com duas tipologias visuais distintas (copas cúbicas e piramidais) e textura de folhagens.
    4. Posiciona o moinho de vento animado e o poço de água nos extremos da vila.
    5. Posiciona a fogueira interativa central na praça.
    6. Instancia seções de cerca (modelo OBJ) delimitando a entrada da vila.
    7. Traça 7 caminhos de pedras procedurais ligando a porta de cada residência até a fogueira central.
  */
void desenhaCena() {
    GLint transformLoc = glGetUniformLocation(Shader_programm, "matriz");
    GLint corLoc = glGetUniformLocation(Shader_programm, "corObjeto");

    // Chão com Textura de Grama repetida
    glm::mat4 matPlano = glm::mat4(1.0f);
    matPlano = glm::scale(matPlano, glm::vec3(10.0f, 1.0f, 10.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matPlano));
    glUniform3f(corLoc, 1.0f, 1.0f, 1.0f); // Sem tintura, textura pura
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexGrama);
    glUniform1i(glGetUniformLocation(Shader_programm, "tex"), 0);

    glBindVertexArray(VaoPlano);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glUniform1i(glGetUniformLocation(Shader_programm, "usaTextura"), 0);
    //Casas
    desenhaCasa(glm::vec3(0.0f, 0.0f, -2.5f), 0.0f, glm::vec3(2.0f));//1
    desenhaCasa(glm::vec3(-2.5f, 0.0f, -1.5f), 45.0f, glm::vec3(1.5f));//2
    desenhaCasa(glm::vec3(2.5f, 0.0f, -1.5f), -45.0f, glm::vec3(1.5f));//3
    desenhaCasa(glm::vec3(-3.5f, 0.0f, 0.5f), 90.0f, glm::vec3(1.3f));//4
    desenhaCasa(glm::vec3(3.5f, 0.0f, 0.5f), -90.0f, glm::vec3(1.3f));//5
    desenhaCasa(glm::vec3(-2.5f, 0.0f, 2.5f), 135.0f, glm::vec3(1.0f));//6
    desenhaCasa(glm::vec3(2.5f, 0.0f, 2.5f), -135.0f, glm::vec3(1.0f));//7

    //árvores piramide e cubo
    desenhaArvoreCubo(glm::vec3(-1.0f,0.0f,-5.5f), 0.0f, glm::vec3(1.0f));//8
    desenhaArvoreCubo(glm::vec3(1.0f,0.0f,-5.5f), 0.0f, glm::vec3(1.0f));//9
    desenhaArvorePiramide(glm::vec3(3.8f,0.0f, -4.2f), 45.0f, glm::vec3(1.5f));//10
    desenhaArvorePiramide(glm::vec3(-3.8f,0.0f, -4.2f), -45.0f, glm::vec3(1.5f));//11
    desenhaArvoreCubo(glm::vec3(5.0f,0.0f,-1.2f), 45.0f, glm::vec3(1.0f));//12
    desenhaArvoreCubo(glm::vec3(-5.0f,0.0f,-1.2f), -45.0f, glm::vec3(1.0f));//13
    desenhaArvorePiramide(glm::vec3(4.5f,0.0f, 3.2f), 75.0f, glm::vec3(1.5f));//14
    desenhaArvorePiramide(glm::vec3(-4.5f,0.0f, 3.2f), -75.0f, glm::vec3(1.5f));//15
    //Moinho cok animação
    desenhaMoinho(glm::vec3(6.0f, 0.0f, 6.0f), -90.0f, glm::vec3(1.2f));//16

    //fogueira com animação de fogo
    desenhaFogueira(glm::vec3(0.0f, 0.0f, 1.0f), 1.5f);//17

    //poço
    desenharPoco(glm::vec3(-6.0f, 0.0f, 6.0f), 90.0f, glm::vec3(1.0f));//18

    // Cerca decorando a entrada e laterais da vila (rotacionadas em 90 graus)
    desenhaCerca(glm::vec3(-1.6f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));
    desenhaCerca(glm::vec3(1.6f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));
    desenhaCerca(glm::vec3(-3.2f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));
    desenhaCerca(glm::vec3(3.2f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));
    desenhaCerca(glm::vec3(-4.8f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));
    desenhaCerca(glm::vec3(4.8f, 0.0f, 4.0f), 90.0f, glm::vec3(1.0f));

    // Caminhos de pedras conectando a porta de cada casa até a praça/fogueira central
    desenhaCaminhoPedra(glm::vec3(0.0f, 0.0f, -1.5f),   glm::vec3(0.0f, 0.0f, 0.1f),   6, 0.35f); // Casa 1 (Norte)
    desenhaCaminhoPedra(glm::vec3(-2.0f, 0.0f, -1.0f),  glm::vec3(-0.6f, 0.0f, 0.4f),  7, 0.30f); // Casa 2 (Noroeste)
    desenhaCaminhoPedra(glm::vec3(2.0f, 0.0f, -1.0f),   glm::vec3(0.6f, 0.0f, 0.4f),   7, 0.30f); // Casa 3 (Nordeste)
    desenhaCaminhoPedra(glm::vec3(-2.85f, 0.0f, 0.5f),  glm::vec3(-0.9f, 0.0f, 0.9f),  7, 0.30f); // Casa 4 (Oeste)
    desenhaCaminhoPedra(glm::vec3(2.85f, 0.0f, 0.5f),   glm::vec3(0.9f, 0.0f, 0.9f),   7, 0.30f); // Casa 5 (Leste)
    desenhaCaminhoPedra(glm::vec3(-2.15f, 0.0f, 2.15f), glm::vec3(-0.7f, 0.0f, 1.5f),  6, 0.28f); // Casa 6 (Sudoeste)
    desenhaCaminhoPedra(glm::vec3(2.15f, 0.0f, 2.15f),  glm::vec3(0.7f, 0.0f, 1.5f),   6, 0.28f); // Casa 7 (Sudeste)
}

/*
  inicializaRenderizacao:
  - Para que serve: Gerencia o loop principal de renderização da aplicação (Game Loop), atualizações por frame e renderização de múltiplas viewports (Cena Principal + Minimapa).
  - O que faz:
    1. Habilita o teste de profundidade (glEnable(GL_DEPTH_TEST)) para oclusão correta entre geometrias 3D.
    2. Loop 'while (!glfwWindowShouldClose)':
       a. Calcula o Delta Time (Tempo_entre_frames) para animações e movimentação suaves.
       b. Limpa os buffers de cor e profundidade.
       c. Processa inputs do teclado (trataTeclado) e orienta a câmera pelo mouse (atualizaDirecaoCamera).
       d. Passagem 1 - Câmera Principal (Perspectiva):
          - Configura glViewport para a janela inteira (0, 0, WIDTH, HEIGHT).
          - Monta a matriz View (glm::lookAt) com posição e direção da câmera do usuário.
          - Monta a matriz Projection com projeção perspectiva (glm::perspective).
          - Renderiza toda a cena através de desenhaCena().
       e. Passagem 2 - Minimapa / Radar (Ortográfica com Múltiplas Viewports):
          - Limpa apenas o Depth Buffer do minimapa (glClear(GL_DEPTH_BUFFER_BIT)).
          - Define uma viewport secundária no canto superior direito da tela (glViewport(miniX, miniY, miniW, miniH)).
          - Configura uma câmera fixa no zênite (Y=15) olhando diretamente para baixo (Top-Down view).
          - Aplica Projeção Ortográfica (glm::ortho) para visualização isométrica sem deformação por perspectiva.
          - Renderiza desenhaCena() novamente na viewport reduzida.
       f. Atualiza buffers e eventos de janela (glfwPollEvents e glfwSwapBuffers).
  */
void inicializaRenderizacao() {
    double tempo_anterior = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(Window)) {
        double tempo_frame_atual = glfwGetTime();
        Tempo_entre_frames = (float)(tempo_frame_atual - tempo_anterior);
        tempo_anterior = tempo_frame_atual;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(Shader_programm);
        
        trataTeclado();
        atualizaDirecaoCamera();

        GLint viewLoc = glGetUniformLocation(Shader_programm, "view");
        GLint projLoc = glGetUniformLocation(Shader_programm, "proj");

        glViewport(0, 0, WIDTH, HEIGHT);

        glm::mat4 view_fps = glm::lookAt(Cam_pos, Cam_pos + Cam_front, Cam_up);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_fps));

        float aspecto_tela = (float)WIDTH / (float)HEIGHT;
        glm::mat4 proj_persp = glm::perspective(glm::radians(Cam_fov), aspecto_tela, 0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_persp));

        desenhaCena();

        glClear(GL_DEPTH_BUFFER_BIT);

        int miniW = 200;
        int miniH = 200;
        int miniX = WIDTH - miniW - 20;
        int miniY = HEIGHT - miniH - 20;
        glViewport(miniX, miniY, miniW, miniH);

        glm::vec3 topo_pos = glm::vec3(0.0f, 15.0f, 0.0f);
        glm::vec3 topo_alvo = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 topo_up = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::mat4 view_mini = glm::lookAt(topo_pos, topo_alvo, topo_up);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_mini));

        float ortho_size = 8.0f;
        glm::mat4 proj_ortho = glm::ortho(-ortho_size, ortho_size, -ortho_size, ortho_size, 0.1f, 30.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_ortho));

        desenhaCena();

        glfwPollEvents();
        glfwSwapBuffers(Window);
    }
    
    glfwTerminate();
}

/*
  main:
  - Para que serve: Ponto de entrada do programa que orquestra a inicialização de subsistemas, carregamento de recursos e início da renderização.
  - O que faz:
    1. Inicializa GLFW, janela e GLAD (inicializaOpenGL).
    2. Gera e sobe para a GPU todas as primitivas geométricas básicas (Plano, Cubo, Pirâmide, Cilindro).
    3. Carrega o modelo poligonal externo OBJ da cerca (carregaOBJ).
    4. Carrega as imagens de textura 2D para VRAM (carregaTextura para grama e folhas).
    5. Compila e vincula os shaders (inicializaShaders).
    6. Dispara o loop de renderização (inicializaRenderizacao).
 */
int main() {
    inicializaOpenGL();
    inicializaPlano();
    inicializaCubo();
    inicializaPiramide();
    inicializaCilindro();
    VaoCerca = carregaOBJ(ASSETS_DIR "/Modelos3D/fence.obj", NumVerticesCerca);
    std::cout << "[OBJ] Cerca carregada com " << NumVerticesCerca << " vertices." << std::endl;
    
    // Carregamento de Texturas 2D (Grama e Folhas)
    TexGrama = carregaTextura(ASSETS_DIR "/tex/grama.jpg");
    TexFolhas = carregaTextura(ASSETS_DIR "/tex/folhas.jpg");

    inicializaShaders();
    inicializaRenderizacao();

    return 0;
}