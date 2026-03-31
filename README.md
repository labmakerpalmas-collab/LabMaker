# Interpretador G-Code Integrado (Projeto Final)

Este projeto evoluiu para um interpretador robusto em C, utilizando parsing por tokens e structs de estado para simular o comportamento de uma máquina CNC.

## 🚀 Funcionalidades Atuais

- **Parsing Robusto:** Utiliza `strtok` para identificar múltiplos comandos e parâmetros em uma mesma linha (Ex: `G1 X150 F1000`).
- **Limpeza de Comentários:** Suporta e remove automaticamente comentários entre parênteses `( )` e estilo ponto e vírgula `;`.
- **Conversão de Unidades (G20/G21):** Identifica e converte automaticamente valores de polegadas para milímetros, mantendo a consistência do sistema.
- **Ciclo de Segurança (150mm):** Valida se os movimentos solicitados cabem no envelope de trabalho de 150mm, emitindo alertas em caso de excesso.
- **Log de Trajetória:** Gera automaticamente um arquivo `trajetoria.txt` contendo as coordenadas `X Y Z` de cada ponto percorrido.
- **Rastreamento de Estado:** Mantém a posição atual, velocidade (feedrate) e modo de coordenadas (absoluto/relativo) em tempo real.

## 📂 Arquivos do Projeto

- `main.c`: Código-fonte principal com o interpretador integrado.
- `test.gcode`: Sequência de teste para o ciclo de 150mm (ida e volta) nos eixos X, Y e Z.
- `trajetoria.txt`: (Gerado após execução) Histórico de coordenadas percorridas.
- `gcode_parser`: Executável final do projeto.

## 🛠️ Como Compilar e Rodar

### 1. Compilação
```bash
gcc main.c -o gcode_parser
```

### 2. Execução
Passe o arquivo G-Code desejado como argumento:
```bash
./gcode_parser test.gcode
```

## 📝 Resultado Esperado no Terminal
O programa detalhará cada movimento, a velocidade utilizada e, ao final, informará a posição de repouso e o modo de coordenadas (`G90` ou `G91`).

---
*Este software foi otimizado para testes de movimentação técnica e validação de curso mecânico.*
