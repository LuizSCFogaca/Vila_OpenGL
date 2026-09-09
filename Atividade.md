# Trabalho de Grau A: Construção de uma Vila Virtual em OpenGL

## Objetivo
Consolidar os fundamentos do pipeline gráfico através da modelagem, posicionamento e visualização de uma cena 3D interativa, aplicando os conceitos de primitivas, transformações e shaders.

---

## Descrição da Cena
O objetivo do trabalho é desenvolver uma **"Vila Virtual"** completa e povoada. Toda a geometria do cenário deve ser construída via código a partir de primitivas básicas (como cubos, pirâmides, planos, cilindros, etc.).

### Regras de Composição
* **Base do Cenário:** É obrigatória a inclusão de um "chão" ou terreno (ex: um plano ou cubo devidamente escalonado).
* **Quantidade Mínima de Objetos:** No mínimo **20 objetos instanciados** no total.
* **Variedade de Elementos:** Distribuídos em pelo menos **6 tipos diferentes** de construções ou elementos (ex: casas, moinhos de vento, árvores, poços, torres, cercas).
* **Criatividade e Coesão:** A temática é livre, mas deve apresentar complexidade e variedade adequadas, focando na justaposição de primitivas básicas para compor objetos complexos e um ambiente coeso.

---

## Requisitos Técnicos

* **Geometria e Composição:**
  * Definição correta dos vértices e índices.
  * As estruturas que compõem a vila devem ser instanciadas e montadas através da manipulação de `VAO`s e `VBO`s.

* **Transformações Geométricas:**
  * A cena deve conter múltiplos elementos instanciados a partir das mesmas geometrias base.
  * Uso obrigatório de **Translação**, **Rotação** e **Escala** (Matriz `Model`) para posicionar as construções pelo cenário, variar seus tamanhos e definir a orientação dos objetos.

* **Câmera e Projeção:**
  * A cena deve ser visualizada utilizando **Projeção Perspectiva**.
  * O sistema deve implementar uma **câmera navegável** (manipulação das matrizes `View` e `Projection`) para permitir a exploração espacial e visualização da vila por diferentes ângulos.

* **Pipeline de Shaders Básicos:**
  * Implementação e uso de **Vertex Shader** e **Fragment Shader**.
  * A distinção visual dos elementos e partes do cenário deve ser feita através da passagem de cores (via atributos de vértice ou variáveis `uniform`).

* **Animação Contínua:**
  * Alguns elementos da cena devem apresentar **animações contínuas e autônomas**, atreladas ao tempo decorrido no loop principal de renderização.
  * O movimento deve ser coerente com a temática do cenário e ter um propósito visual dentro da composição (ex: as pás de um moinho girando, um corpo celeste orbitando a vila, ou um portão abrindo e fechando), evitando animações arbitrárias inseridas unicamente para cumprir a exigência técnica.

* **Interação do Usuário:**
  * O sistema deve capturar eventos de entrada (teclado ou mouse) para alterar dinamicamente algum estado da cena, além da movimentação da câmera (ex: alternar esquemas de cores do cenário, interagir com partes das construções ou alterar modos de visualização).

* **Carregamento de Modelos 3D:**
  * Substituir a construção manual de alguns objetos pela importação de modelos externos simples no formato `.obj`.
  * Isso exige a escrita ou integração de um parser para ler os vértices, normais e índices do arquivo e alocá-los corretamente nos `VBO`s.

* **Mapeamento de Texturas (2D):**
  * Aplicar texturas nos objetos da cena.
  * Requer o mapeamento correto de coordenadas UV nos vértices e a configuração de `Samplers` no Fragment Shader.

* **Minimapa (Múltiplas Viewports):**
  * Utilizar o comando `glViewport` para renderizar a cena uma segunda vez em um canto da janela.
  * Empregar uma câmera ortográfica posicionada acima da vila olhando para baixo, criando um "radar" de navegação.

---

## Entrega e Apresentação
* **Código-Fonte:** Submissão do código-fonte completo do projeto.
* **Apresentação Individual:** Apresentação individual obrigatória para o professor, demonstrando a vila virtual em execução e explicando as soluções técnicas e a matemática utilizada no desenvolvimento.

---

## Extras (Desafios Bônus)
> *A implementação dos itens abaixo é opcional e serve para recuperar notas futuras:*

1. **Iluminação Básica:**
   * Implementar um modelo de reflexão local (como Phong ou Gouraud) com pelo menos uma fonte de luz (ex: luz direcional simulando o sol).
   * Exige a definição e o envio dos vetores normais das faces e o cálculo matemático da iluminação nos shaders.

2. **Renderização Instanciada (*Instancing*):**
   * Utilizar comandos como `glDrawArraysInstanced` ou `glDrawElementsInstanced` para renderizar os objetos repetidos do cenário (como árvores ou cercas) em uma única *Draw Call*, gerenciando as matrizes de transformação diretamente na GPU.

3. **Otimização de Desempenho (*Frustum Culling*):**
   * Implementação de um algoritmo de *Frustum Culling* (Descarte de Volume de Visualização) baseado nos 6 planos de corte extraídos do produto das matrizes `Projection * View`.
   * O sistema deve testar a *Bounding Box* ou *Bounding Sphere* dos objetos da vila antes de enviá-los para a renderização, descartando do pipeline de desenho tudo o que estiver fora do campo de visão da câmera para otimizar o desempenho do loop principal.

---

## Aviso Importante sobre as Implementações

> [!IMPORTANT]
> A entrega do trabalho exige que você tenha **domínio completo sobre o código desenvolvido**. Não basta apenas copiar soluções prontas da internet. Durante a avaliação, você deverá demonstrar que compreende as técnicas implementadas, a matemática envolvida e como o seu código interage com o pipeline gráfico.

> [!WARNING]
> **Atenção aos Extras:** Essa regra vale especialmente para os desafios bônus, já que eles exigem um estudo autônomo de conceitos que vão além do que vimos em sala de aula.
> 
> Qualquer trecho de código (seja dos requisitos obrigatórios ou dos extras) que seja incluído no projeto sem que você consiga explicar claramente como ele funciona **será desconsiderado da sua nota**.