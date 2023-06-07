#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_PAGES 100
#define N_FRAMES 10
#define N_LOOPS 50
#define START_LOOPS 10
#define END_LOOPS 90

typedef struct {
    int id;
    int r_bit;
    int m_bit;
} Page;

void nru_page_replacement() {
    Page page_table[N_PAGES];
    int frame_table[N_FRAMES];
    int frame_counter = 0;
    int page_faults = 0;

    srand(time(NULL));

    for (int i = 0; i < N_PAGES; i++) {
        page_table[i].id = i;
        page_table[i].r_bit = 0;
        page_table[i].m_bit = 0;
    }

    for (int i = 0; i < N_LOOPS; i++) {
        int page_number = rand() % (END_LOOPS - START_LOOPS + 1) + START_LOOPS;

        Page* page = &page_table[page_number];

        if (frame_counter < N_FRAMES) {
            // Ainda há espaço nos quadros de página
            frame_table[frame_counter] = page_number;
            frame_counter++;
        } else {
            // Todos os quadros de página estão ocupados
            int class_bits[4][N_FRAMES]; // Classes de bits: 00, 01, 10, 11
            int class_counter[4] = {0};

            for (int j = 0; j < N_FRAMES; j++) {
                Page* frame_page = &page_table[frame_table[j]];
                int class_index = (frame_page->r_bit << 1) | frame_page->m_bit;
                class_bits[class_index][class_counter[class_index]] = j;
                class_counter[class_index]++;
            }

            int victim_class = -1;
            int victim_index = -1;

            // Escolhe aleatoriamente uma classe com prioridade da menor para a maior
            for (int j = 0; j < 4; j++) {
                if (class_counter[j] > 0) {
                    victim_class = j;
                    break;
                }
            }

            if (victim_class == -1) {
                // Todas as classes estão vazias
                // Seleciona aleatoriamente qualquer página
                victim_class = rand() % 4;
            }

            // Escolhe aleatoriamente uma página dentro da classe selecionada
            victim_index = class_bits[victim_class][rand() % class_counter[victim_class]];

            // Substitui a página vítima pelo novo número de página
            frame_table[victim_index] = page_number;
        }

        page->r_bit = 1; // Define o bit R como 1 para indicar que foi referenciada
        page->m_bit = rand() % 2; // Define o bit M aleatoriamente como 0 ou 1 para indicar se foi modificada

        page_faults++;

        printf("Page Table:\n");
        for (int j = 0; j < N_PAGES; j++) {
            printf("Page %d: R=%d M=%d\n", page_table[j].id, page_table[j].r_bit, page_table[j].m_bit);
        }

        printf("Frame Table: ");
        for (int j = 0; j < frame_counter; j++) {
            printf("%d ", frame_table[j]);
        }
        printf("\n\n");
    }

    printf("Total Page Faults: %d\n", page_faults);
}

int main() {
    nru_page_replacement();

    return 0;
}
