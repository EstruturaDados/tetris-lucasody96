#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

// --- Constantes de Configuração ---
#define TAM_FILA 5    // Capacidade da Fila Circular
#define TAM_PILHA 3   // Capacidade da Pilha de Reserva

// --- Estruturas de Dados ---

// Representação da Peça
typedef struct {
    char nome;  // Tipo: 'I', 'O', 'T', 'L', etc.
    int id;     // Identificador único
} Peca;

// Estrutura de Fila Circular
typedef struct {
    Peca itens[TAM_FILA];
    int inicio; // Índice da frente
    int fim;    // Índice de inserção
    int qtd;    // Quantidade atual
} FilaCircular;

// Estrutura de Pilha (LIFO)
typedef struct {
    Peca itens[TAM_PILHA];
    int topo;   // Índice do topo (-1 se vazia)
} Pilha;

// --- Variável Global para IDs ---
int idGlobal = 0;

// --- Protótipos das Funções ---
void inicializarFila(FilaCircular *f);
void inicializarPilha(Pilha *p);
Peca gerarPeca();

// Primitivas de Estrutura
void enqueue(FilaCircular *f, Peca p);
Peca dequeue(FilaCircular *f);
int push(Pilha *p, Peca item);
int pop(Pilha *p, Peca *itemRetorno);

// Ações do Jogo (Requisitos Funcionais)
void acaoJogar(FilaCircular *f);
void acaoReservar(FilaCircular *f, Pilha *p);
void acaoUsarReserva(Pilha *p);
void acaoTrocarUm(FilaCircular *f, Pilha *p);
void acaoTrocarBloco(FilaCircular *f, Pilha *p);

// Visualização
void exibirEstado(FilaCircular *f, Pilha *p);
void limparBuffer();

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL)); 

    FilaCircular fila;
    Pilha pilha;
    int opcao;

    // 1. Inicialização
    inicializarFila(&fila);
    inicializarPilha(&pilha);

    // 2. Pré-carregamento da fila (Mantendo-a cheia inicialmente)
    for(int i = 0; i < TAM_FILA; i++) {
        enqueue(&fila, gerarPeca());
    }

    // 3. Loop do Jogo
    do {
        exibirEstado(&fila, &pilha);

        printf("\nOpções disponíveis:\n");
        printf("1 - Jogar peça da frente da fila\n");
        printf("2 - Enviar peça da fila para a pilha de reserva\n");
        printf("3 - Usar peça da pilha de reserva\n");
        printf("4 - Trocar peça da frente da fila com o topo da pilha\n");
        printf("5 - Trocar os 3 primeiros da fila com as 3 peças da pilha\n");
        printf("0 - Sair\n");
        printf("Opção escolhida: ");
        scanf("%d", &opcao);
        limparBuffer();

        printf("\n--------------------------------------------------\n");

        switch(opcao) {
            case 1: acaoJogar(&fila); break;
            case 2: acaoReservar(&fila, &pilha); break;
            case 3: acaoUsarReserva(&pilha); break;
            case 4: acaoTrocarUm(&fila, &pilha); break;
            case 5: acaoTrocarBloco(&fila, &pilha); break;
            case 0: printf("Encerrando o sistema...\n"); break;
            default: printf("[!] Opção inválida.\n");
        }
        printf("--------------------------------------------------\n");

    } while(opcao != 0);

    return 0;
}

// ============================================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================================================

// --- Gerador ---
Peca gerarPeca() {
    Peca p;
    char tipos[] = {'I', 'O', 'T', 'L', 'Z', 'S', 'J'};
    p.nome = tipos[rand() % 7];
    p.id = idGlobal++;
    return p;
}

// --- Inicialização ---
void inicializarFila(FilaCircular *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;
}
void inicializarPilha(Pilha *p) {
    p->topo = -1;
}

// --- Primitivas Fila/Pilha ---
void enqueue(FilaCircular *f, Peca p) {
    if (f->qtd < TAM_FILA) {
        f->itens[f->fim] = p;
        f->fim = (f->fim + 1) % TAM_FILA; // Lógica circular
        f->qtd++;
    }
}

Peca dequeue(FilaCircular *f) {
    Peca p = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA; // Lógica circular
    f->qtd--;
    return p;
}

int push(Pilha *p, Peca item) {
    if (p->topo >= TAM_PILHA - 1) return 0; // Full
    p->topo++;
    p->itens[p->topo] = item;
    return 1;
}

int pop(Pilha *p, Peca *itemRetorno) {
    if (p->topo == -1) return 0; // Empty
    *itemRetorno = p->itens[p->topo];
    p->topo--;
    return 1;
}

// --- AÇÕES DO JOGO ---

// 1. Jogar: Remove da fila e repõe no final
void acaoJogar(FilaCircular *f) {
    Peca p = dequeue(f);
    printf(">> Peça JOGADA: [%c %d]\n", p.nome, p.id);
    enqueue(f, gerarPeca()); // Reposição automática
}

// 2. Reservar: Tira da fila, põe na pilha, repõe fila
void acaoReservar(FilaCircular *f, Pilha *p) {
    if (p->topo >= TAM_PILHA - 1) {
        printf("[!] A reserva está cheia! Não é possível reservar.\n");
        return;
    }
    Peca pFila = dequeue(f);
    push(p, pFila);
    printf(">> Peça RESERVADA: [%c %d]\n", pFila.nome, pFila.id);
    enqueue(f, gerarPeca()); // Reposição automática
}

// 3. Usar Reserva: Remove do topo da pilha (não afeta fila)
void acaoUsarReserva(Pilha *p) {
    Peca pPilha;
    if (pop(p, &pPilha)) {
        printf(">> Usando peça da RESERVA: [%c %d]\n", pPilha.nome, pPilha.id);
    } else {
        printf("[!] Reserva vazia.\n");
    }
}

// 4. Troca Simples: Frente da Fila <-> Topo da Pilha
void acaoTrocarUm(FilaCircular *f, Pilha *p) {
    if (p->topo == -1) {
        printf("[!] Impossível trocar: Reserva vazia.\n");
        return;
    }
    
    // Acesso direto aos ponteiros para troca (Swap)
    // Na fila circular, o índice real é 'inicio'
    // Na pilha, o índice real é 'topo'
    Peca temp = f->itens[f->inicio];
    f->itens[f->inicio] = p->itens[p->topo];
    p->itens[p->topo] = temp;
    
    printf(">> Troca efetuada: [%c %d] <-> [%c %d]\n", 
           p->itens[p->topo].nome, p->itens[p->topo].id, // Antigo da fila, agora na pilha
           f->itens[f->inicio].nome, f->itens[f->inicio].id); // Antigo da pilha, agora na fila
}

// 5. Troca Múltipla (Avançado)
// Troca os 3 primeiros da fila com os 3 da pilha
void acaoTrocarBloco(FilaCircular *f, Pilha *p) {
    // Validação: Pilha precisa ter 3 itens. Fila sempre tem 5, então ok.
    if (p->topo < 2) { // Índices 0, 1, 2 devem existir
        printf("[!] Ação negada: A pilha precisa ter 3 peças para a troca múltipla.\n");
        return;
    }

    printf(">> Realizando TROCA MÚLTIPLA (3 peças)...\n");

    // Loop para trocar 3 itens
    // i=0: Topo da pilha <-> Início da Fila
    // i=1: Meio da pilha <-> Início+1 da Fila
    // i=2: Base da pilha <-> Início+2 da Fila
    for (int i = 0; i < 3; i++) {
        // Cálculo do índice na fila circular
        int idxFila = (f->inicio + i) % TAM_FILA;
        // Cálculo do índice na pilha (Topo descendo)
        int idxPilha = p->topo - i;

        // Swap
        Peca temp = f->itens[idxFila];
        f->itens[idxFila] = p->itens[idxPilha];
        p->itens[idxPilha] = temp;
    }
    printf(">> Troca em bloco realizada com sucesso!\n");
}

// --- Visualização ---
void exibirEstado(FilaCircular *f, Pilha *p) {
    printf("\nEstado atual:\n");
    
    // Fila
    printf("Fila de peças: ");
    for(int i = 0; i < f->qtd; i++) {
        int idx = (f->inicio + i) % TAM_FILA;
        printf("[%c %d] ", f->itens[idx].nome, f->itens[idx].id);
    }
    printf("\n");

    // Pilha
    printf("Pilha de reserva (Topo -> Base): ");
    if (p->topo == -1) printf("[ VAZIA ]");
    else {
        for(int i = p->topo; i >= 0; i--) {
            printf("[%c %d] ", p->itens[i].nome, p->itens[i].id);
        }
    }
    printf("\n");
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}