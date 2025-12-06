Principais contribuições do Felipe: desenvolvimento do sistema de câmeras, do sistema de objetos virtuais e do sistema de componentes, além da implementação dos inimigos no planeta, da física da nave e do jogador, das partículas da nave, do sistema de colisões, da lógica de combustível e do sistema de cenas. Contribuições do Fabrício: envolvem a seleção de modelos e texturas, o controle de input, a criação da textura procedural dos planetas, o uso de curvas de Bézier, o desenvolvimento de shaders, o sistema de trajetórias fixas, a lógica de oxigênio e as estrelas. Vale destacar que essas são as áreas em que um dos membros contribuiu significativamente mais do que o outro, pois em grande parte do código ambos trabalharam juntos. Outras contribuições menos relevantes ou realizadas de forma equivalente entre os dois foram omitidas.

A ferramenta Github Copilot foi utilizada por ambos os membros durante a escrita do código e eventuais sugestões foram acatadas, incluindo para funções pequenas com escopo bem definido. Além disso, funcionalidades mais complexas foram auxiliadas por LLMs através de explicações teóricas. Também experimentamos a funcionalidade de associar uma issue ao Copilot, todavia, a maior parte dos Pull Requests foi rejeitada pois não se adequava ao que havia sido pedido no prompt. Por fim, o Copilot Chat foi muito útil para debug.

Os conceitos da disciplina de Fundamentos de Computação Gráfica foram bastante utilizados nos laboratórios e nós os reutilizamos para implementar as funcionalidades mais essenciais, principalmente na engine. O início do desenvolvimento utilizou principalmente os laboratórios 1 e 2, de onde reutilizamos os conceitos de criação de VAOs, câmeras, matrizes de transformação e projeção, etc. O conceito de transformações hierárquicas explorados no laboratório 3 foram utilizados na lógica de Transform de objetos virtuais da engine. Posteriormente, utilizamos os conceitos de iluminação, texturas e interpolação que foram praticados nos laboratórios 4 e 5. As aulas de Bézier foram úteis para a explicação de conceitos utilizados na implementação dessas curvas no jogo. Por fim, as aulas de animações também foram importantes para o jogo, já que a nave e o jogador utilizam movimentação com aceleração, conforme visto em aula. 

Como jogar?

Trocar para camera livre no jogador: F6
Movimento do jogador: W, A, S, D
Pulo do jogador: Espaço
Rotacionar câmera no jogador: mouse 
Trocar entre jogador humanoide e nave: E
Acelerar a nave: W
Girar a nave (eixo z): A e D
Rotacionar nave (eixos x e y): mouse

Como rodar?

$sudo apt-get install build-essential make libx11-dev libxrandr-dev \
                         libxinerama-dev libxcursor-dev libxcb1-dev libxext-dev \
                         libxrender-dev libxfixes-dev libxau-dev libxdmcp-dev
$make clean # Se necessario
$make run

vscode:

Instale as extensões "ms-vscode.cpptools" e "ms-vscode.cmake-tools"
Clique no botão play no canto inferior direito.
	
