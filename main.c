#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256

typedef struct {
    float posX;
    float posY;
    float posZ;
    float feedRate;
    int modoAbsoluto; 
    int unidadeMM;   
} EstadoMaquina;

typedef struct {
    int movimento;
    int temX, temY, temZ, temF;
    float X, Y, Z, F;
} ComandoGcode;

void inicializarMaquina(EstadoMaquina *maq) {
    maq->posX = 0;
    maq->posY = 0;
    maq->posZ = 0;
    maq->feedRate = 0;
    maq->modoAbsoluto = 1;
    maq->unidadeMM = 1; 
}

float converterParaMM(float valor, EstadoMaquina *maq) {
    if (maq->unidadeMM) return valor;
    return valor * 25.4;
}

void removerComentarios(char *linha) {
    char resultado[MAX_LINE];
    int i = 0, j = 0;
    int comentario = 0;

    while (linha[i] != '\0') {
        if (linha[i] == '(') comentario = 1;
        else if (linha[i] == ')') {
            comentario = 0;
            i++;
            continue;
        }
        if (!comentario && linha[i] != ';') {
            resultado[j++] = linha[i];
        }
        if (linha[i] == ';') break; // Suporte para comentário estilo ';'
        i++;
    }
    resultado[j] = '\0';
    strcpy(linha, resultado);
}

void limparComando(ComandoGcode *cmd) {
    cmd->movimento = -1;
    cmd->temX = cmd->temY = cmd->temZ = cmd->temF = 0;
}

void salvarTrajetoria(FILE *log, EstadoMaquina *maq) {
    if (log) fprintf(log, "%.2f %.2f %.2f\n", maq->posX, maq->posY, maq->posZ);
}

void interpretarLinha(char *linha, EstadoMaquina *maq, FILE *log) {
    ComandoGcode cmd;
    limparComando(&cmd);

    char linhaCopia[MAX_LINE];
    strcpy(linhaCopia, linha);
    char *token = strtok(linhaCopia, " \t\n\r");

    while (token != NULL) {
        char letra = toupper(token[0]);
        if (letra == 'G') {
            int codigo = atoi(&token[1]);
            if (codigo == 0) cmd.movimento = 0;
            else if (codigo == 1) cmd.movimento = 1;
            else if (codigo == 90) maq->modoAbsoluto = 1;
            else if (codigo == 91) maq->modoAbsoluto = 0;
            else if (codigo == 20) maq->unidadeMM = 0;
            else if (codigo == 21) maq->unidadeMM = 1;
        }
        else if (letra == 'X') { cmd.X = converterParaMM(atof(&token[1]), maq); cmd.temX = 1; }
        else if (letra == 'Y') { cmd.Y = converterParaMM(atof(&token[1]), maq); cmd.temY = 1; }
        else if (letra == 'Z') { cmd.Z = converterParaMM(atof(&token[1]), maq); cmd.temZ = 1; }
        else if (letra == 'F') { maq->feedRate = converterParaMM(atof(&token[1]), maq); cmd.temF = 1; }

        token = strtok(NULL, " \t\n\r");
    }

    if (cmd.movimento != -1 || cmd.temX || cmd.temY || cmd.temZ) {
        float nX = maq->posX, nY = maq->posY, nZ = maq->posZ;

        if (maq->modoAbsoluto) {
            if (cmd.temX) nX = cmd.X;
            if (cmd.temY) nY = cmd.Y;
            if (cmd.temZ) nZ = cmd.Z;
        } else {
            if (cmd.temX) nX += cmd.X;
            if (cmd.temY) nY += cmd.Y;
            if (cmd.temZ) nZ += cmd.Z;
        }

        // Alerta de segurança (150mm)
        if (nX > 150.0 || nY > 150.0 || nZ > 150.0) {
            printf("[ALERTA] %.1fmm excede o limite de 150mm!\n", (nX > 150.0 ? nX : (nY > 150.0 ? nY : nZ)));
        }

        printf(" -> X:%.1f Y:%.1f Z:%.1f | Vel:%.1f\n", nX, nY, nZ, maq->feedRate);
        
        maq->posX = nX; maq->posY = nY; maq->posZ = nZ;
        salvarTrajetoria(log, maq);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.gcode>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (!arquivo) {
        printf("Erro ao abrir %s\n", argv[1]);
        return 1;
    }

    FILE *log = fopen("trajetoria.txt", "w");
    EstadoMaquina maquina;
    inicializarMaquina(&maquina);

    printf("=== Interpretador G-code Integrado (150mm) ===\n");

    char linha[MAX_LINE];
    while (fgets(linha, MAX_LINE, arquivo)) {
        removerComentarios(linha);
        if (strlen(linha) > 1) {
            interpretarLinha(linha, &maquina, log);
        }
    }

    printf("\nPosicao final: X=%.2f Y=%.2f Z=%.2f mm\n", maquina.posX, maquina.posY, maquina.posZ);
    printf("Modo Final: %s\n", maquina.modoAbsoluto ? "absoluto (G90)" : "relativo (G91)");

    fclose(arquivo);
    if (log) fclose(log);

    return 0;
}
