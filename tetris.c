#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

// --- Configurações do Jogo ---
#define TAM_FILA 5    // A fila de próximas peças é fixa em 5
#define TAM_PILHA 3   // A reserva aguenta até 3 peças

// --- Estruturas de Dados ---

// Representação de uma Peça
typedef struct {
    char nome;  // Tipo: 'I', 'O', 'T', 'L', etc.
    int id;     // Identificador único
} Peca;

// Estrutura de Fila Circular
typedef struct {
    Peca itens[TAM_FILA];
    int inicio;
    int fim;
    int qtd;
} FilaCircular;

// Estrutura de Pilha (LIFO - Last In, First Out)
typedef struct {
    Peca itens[TAM_PILHA];
    int topo; // Índice do elemento no topo (-1 se vazia)
} Pilha;

// --- Protótipos das Funções ---
void inicializarFila(FilaCircular *f);
void inicializarPilha(Pilha *p);
Peca gerarPeca(int *idGlobal);

// Operações de Fila (Auxiliares)
void enqueue(FilaCircular *f, Peca p); // Inserir
Peca dequeue(FilaCircular *f);         // Remover

// Operações de Pilha (Auxiliares)
int push(Pilha *p, Peca item);         // Empilhar
int pop(Pilha *p, Peca *itemRetorno);  // Desempilhar

// Ações do Jogo
void jogarPeca(FilaCircular *f, int *idGlobal);
void reservarPeca(FilaCircular *f, Pilha *p, int *idGlobal);
void usarPecaReserva(Pilha *p);
void exibirEstado(FilaCircular *f, Pilha *p);

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL)); // Semente para números aleatórios

    FilaCircular fila;
    Pilha pilha;
    int idGlobal = 0; // Contador único para as peças
    int opcao;

    // 1. Inicialização
    inicializarFila(&fila);
    inicializarPilha(&pilha);

    // 2. Pré-aquecimento: Encher a fila inicial
    printf("Gerando peças iniciais...\n");
    for(int i = 0; i < TAM_FILA; i++) {
        enqueue(&fila, gerarPeca(&idGlobal));
    }

    // 3. Loop do Jogo
    do {
        exibirEstado(&fila, &pilha);

        printf("\nOpções de Ação:\n");
        printf("1 - Jogar peça (Fila -> Jogo)\n");
        printf("2 - Reservar peça (Fila -> Pilha)\n");
        printf("3 - Usar peça reservada (Pilha -> Jogo)\n");
        printf("0 - Sair\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        // Limpeza de buffer (para evitar bugs de 'enter')
        while(getchar() != '\n'); 
        printf("\n--------------------------------------------------\n");

        switch(opcao) {
            case 1:
                jogarPeca(&fila, &idGlobal);
                break;
            case 2:
                reservarPeca(&fila, &pilha, &idGlobal);
                break;
            case 3:
                usarPecaReserva(&pilha);
                break;
            case 0:
                printf("Encerrando Tetris Stack...\n");
                break;
            default:
                printf("[!] Opção inválida.\n");
        }

    } while(opcao != 0);

    return 0;
}

// ============================================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================================================

// --- Gerador de Peças ---
Peca gerarPeca(int *idGlobal) {
    Peca p;
    char tipos[] = {'I', 'O', 'T', 'L', 'Z', 'S', 'J'};
    p.nome = tipos[rand() % 7];
    p.id = *idGlobal;
    (*idGlobal)++; // Incrementa o ID para a próxima chamada
    return p;
}

// --- Funções de Inicialização ---
void inicializarFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;
}

void inicializarPilha(Pilha *p) {
    p->topo = -1; // -1 indica pilha vazia
}

// --- Primitivas da Fila Circular ---
void enqueue(FilaCircular *f, Peca p) {
    // Nota: Neste jogo, como removemos e inserimos imediatamente,
    // a fila nunca deve estourar o limite se a lógica estiver certa.
    if (f->qtd < TAM_FILA) {
        f->itens[f->fim] = p;
        f->fim = (f->fim + 1) % TAM_FILA; // Aritmética modular (Circular)
        f->qtd++;
    }
}

Peca dequeue(FilaCircular *f) {
    Peca p = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA; // Move o início circularmente
    f->qtd--;
    return p;
}

// --- Primitivas da Pilha ---
int push(Pilha *p, Peca item) {
    if (p->topo >= TAM_PILHA - 1) {
        return 0; // Erro: Pilha cheia (Stack Overflow)
    }
    p->topo++;
    p->itens[p->topo] = item;
    return 1; // Sucesso
}

int pop(Pilha *p, Peca *itemRetorno) {
    if (p->topo == -1) {
        return 0; // Erro: Pilha vazia (Stack Underflow)
    }
    *itemRetorno = p->itens[p->topo];
    p->topo--;
    return 1; // Sucesso
}

// --- Lógica do Jogo ---

// Ação 1: Jogar
// Remove da frente da fila e IMEDIATAMENTE repõe uma nova no final
void jogarPeca(FilaCircular *f, int *idGlobal) {
    Peca jogada = dequeue(f);
    printf("[AÇÃO] Peça '%c' (ID %d) foi jogada no campo!\n", jogada.nome, jogada.id);
    
    // Regra: Manter fila sempre cheia
    Peca nova = gerarPeca(idGlobal);
    enqueue(f, nova);
    printf("[AUTO] Nova peça '%c' entrou na fila.\n", nova.nome);
}

// Ação 2: Reservar
// Tira da fila, tenta colocar na pilha. Se der certo, repõe a fila.
void reservarPeca(FilaCircular *f, Pilha *p, int *idGlobal) {
    // Verifica se a pilha cabe mais alguém antes de tirar da fila
    if (p->topo >= TAM_PILHA - 1) {
        printf("[ERRO] Reserva cheia! Jogue ou use uma peça reservada.\n");
        return;
    }

    Peca aReservar = dequeue(f);
    push(p, aReservar);
    printf("[AÇÃO] Peça '%c' (ID %d) movida para a RESERVA.\n", aReservar.nome, aReservar.id);

    // Regra: Manter fila sempre cheia
    Peca nova = gerarPeca(idGlobal);
    enqueue(f, nova);
    printf("[AUTO] Nova peça '%c' entrou na fila para compensar.\n", nova.nome);
}

// Ação 3: Usar Reserva
// Apenas remove do topo da pilha e joga. Não afeta a fila.
void usarPecaReserva(Pilha *p) {
    Peca usada;
    if (pop(p, &usada)) {
        printf("[AÇÃO] Usando peça RESERVADA '%c' (ID %d)!\n", usada.nome, usada.id);
    } else {
        printf("[ERRO] Não há peças na reserva.\n");
    }
}

// Exibição Visual
void exibirEstado(FilaCircular *f, Pilha *p) {
    printf("\nEstado atual:\n");
    
    // 1. Mostrar Fila
    printf("Fila de peças: ");
    for (int i = 0; i < f->qtd; i++) {
        // Cálculo do índice real na fila circular
        int idx = (f->inicio + i) % TAM_FILA;
        printf("[%c %d] ", f->itens[idx].nome, f->itens[idx].id);
    }
    printf("\n");

    // 2. Mostrar Pilha
    // Mostra do Topo para a Base (LIFO visual)
    printf("Pilha de reserva (Topo -> Base): ");
    if (p->topo == -1) {
        printf("[ VAZIA ]");
    } else {
        for (int i = p->topo; i >= 0; i--) {
            printf("[%c %d] ", p->itens[i].nome, p->itens[i].id);
        }
    }
    printf("\n");
}