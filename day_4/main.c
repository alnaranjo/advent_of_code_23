#include <file.h>
#include <set.h>
#include <stdio.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

#define BUFFER_MAX_LENGTH 128

typedef struct {
    int id;
    int total_winning_numbers;
    int total_playing_numbers;
    int *playing_numbers;
    int *winning_numbers;
} Card;

Card **parse_cards(FileContents *file_contents, int *total_parsed);
Card *parse_card(char *str);
void destroy_cards(Card **cards, int total);
void destroy_card(Card *card);
int *parse_numbers(char *str, int *total_parsed);
int calculate_cards_points(Card **cards, int total);
int calculate_card_points(Card *card);
int calculate_total_winning_numbers(Card *card);
int *calculate_total_cards_won(Card **cards, int total);
void print_cards(Card **cards, int total);
void print_card(Card card);

int main(int argc, char **argv) {
    printf("Day 4\n");

    if (argc != 2) {
        fprintf(stderr, "ERROR: Missing data filename\n");
        printf("Ussage: ./main <data_filename>\n");
        return 1;
    }

    const char *filename = argv[1];

    FileContents *file_contents = read_file_contents(filename);
    if (file_contents == NULL) {
        fprintf(stderr, "ERROR: Unable to read file %s\n", filename);
        return 2;
    }

    int total_cards;
    Card **cards = parse_cards(file_contents, &total_cards);
    if (cards == NULL) {
        fprintf(stderr, "ERROR: Unable to parse cards\n");
        return 3;
    }

    int total_part_1 = calculate_cards_points(cards, total_cards);
    printf("part_1_total: %d\n", total_part_1);

    int *total_won_cards = calculate_total_cards_won(cards, total_cards);
    if (total_won_cards == NULL) {
        fprintf(stderr, "ERROR: Unable to calculate total cards won");
        return 4;
    }

    int total_part_2 = 0;
    for (int i = 0; i < total_cards; ++i) {
        total_part_2 += total_won_cards[i];
    }
    printf("part_2_total: %d\n", total_part_2);

    free(total_won_cards);
    destroy_cards(cards, total_cards);
    destroy_file_contents(file_contents);

    return 0;
}

Card **parse_cards(FileContents *file_contents, int *total_parsed) {
    if (file_contents == NULL || total_parsed == NULL) {
        return NULL;
    }

    *total_parsed = 0;
    Card **cards = malloc(sizeof(Card *) * file_contents->count);

    for (int i = 0; i < file_contents->count; ++i) {
        char *str = file_contents->values[i];
        Card *card = parse_card(str);
        if (card == NULL) {
            free(cards);
            return NULL;
        }
        cards[i] = card;
    }

    *total_parsed = file_contents->count;
    return cards;
}

Card *parse_card(char *str) {
    if (str == NULL) {
        return NULL;
    }

    int total_substrings;
    char **split = split_string(str, ":", &total_substrings);
    if (total_substrings != 2) {
        return NULL;
    }
    char *card_info = strdup(split[0]);
    char *numbers_info = strdup(split[1]);

    split = split_string(card_info, " ", &total_substrings);
    if (total_substrings != 2) {
        return NULL;
    }
    int id = atoi(split[1]);

    split = split_string(numbers_info, "|", &total_substrings);
    if (total_substrings != 2) {
        return NULL;
    }
    char *winning_numbers_data = strdup(split[0]);
    char *playing_numbers_data = strdup(split[1]);

    int total_winning_numbers;
    int total_playing_numbers;
    int *winning_numbers = parse_numbers(winning_numbers_data, &total_winning_numbers);
    int *playing_numbers = parse_numbers(playing_numbers_data, &total_playing_numbers);

    if (winning_numbers == NULL || playing_numbers == NULL) {
        return NULL;
    }

    Card *card = malloc(sizeof(Card));
    if (card == NULL) {
        return NULL;
    }

    card->id = id;
    card->winning_numbers = winning_numbers;
    card->playing_numbers = playing_numbers;
    card->total_winning_numbers = total_winning_numbers;
    card->total_playing_numbers = total_playing_numbers;

    free(playing_numbers_data);
    free(winning_numbers_data);
    free(numbers_info);
    free(card_info);

    return card;
}

void destroy_cards(Card **cards, int total) {
    if (cards == NULL) {
        return;
    }

    for (int i = 0; i < total; ++i) {
        destroy_card(cards[i]);
    }

    free(cards);
    cards = NULL;
}

void destroy_card(Card *card) {
    if (card == NULL) {
        return;
    }

    if (card->winning_numbers) {
        free(card->winning_numbers);
        card->winning_numbers = NULL;
    }

    if (card->playing_numbers) {
        free(card->playing_numbers);
        card->playing_numbers = NULL;
    }

    free(card);
    card = NULL;
}

int *parse_numbers(char *str, int *total_parsed) {
    if (str == NULL || total_parsed == NULL) {
        return NULL;
    }

    *total_parsed = 0;

    int total_substrings;
    char **split = split_string(str, " ", &total_substrings);
    if (split == NULL || total_substrings == 0) {
        return NULL;
    }

    int *results = malloc(sizeof(int) * total_substrings);
    for (int i = 0; i < total_substrings; ++i) {
        results[i] = atoi(split[i]);
    }

    *total_parsed = total_substrings;
    return results;
}

int calculate_cards_points(Card **cards, int total) {
    if (cards == NULL) {
        return 0;
    }

    int total_points = 0;
    for (int i = 0; i < total; ++i) {
        total_points += calculate_card_points(cards[i]);
    }

    return total_points;
}
int calculate_card_points(Card *card) {
    int total_matches = 0;
    int total_points = 0;

    for (int i = 0; i < card->total_playing_numbers; ++i) {
        for (int j = 0; j < card->total_winning_numbers; ++j) {
            if (card->playing_numbers[i] == card->winning_numbers[j]) {
                ++total_matches;
                if (total_matches == 1) {
                    total_points = 1;
                } else {
                    total_points *= 2;
                }
            }
        }
    }

    return total_points;
}

int calculate_total_winning_numbers(Card *card) {
    int total = 0;

    for (int i = 0; i < card->total_playing_numbers; ++i) {
        for (int j = 0; j < card->total_winning_numbers; ++j) {
            if (card->playing_numbers[i] == card->winning_numbers[j]) {
                ++total;
            }
        }
    }

    return total;
}

int *calculate_total_cards_won(Card **cards, int total) {
    if (cards == NULL) {
        return NULL;
    }

    int *total_winnings = malloc(sizeof(int) * total);

    if (total_winnings == NULL) {
        fprintf(stderr, "ERROR: Unable to alloc memory");
        return NULL;
    }

    for (int i = 0; i < total; ++i) {
        total_winnings[i] += 1;

        int wins = calculate_total_winning_numbers(cards[i]);
        int start = i + 1;
        int end = start + wins;

        for (int j = start; j < end; ++j) {
            total_winnings[j] += total_winnings[i];
        }
    }

    return total_winnings;
}

void print_cards(Card **cards, int total) {
    if (cards == NULL || total == 0) {
        return;
    }
    for (int i = 0; i < total; ++i) {
        print_card(*cards[i]);
    }
}

void print_card(Card card) {
    printf("id: %d\n", card.id);
    printf("total_winning_numbers: %d\n", card.total_winning_numbers);
    printf("[");
    for (int i = 0; i < card.total_winning_numbers; ++i) {
        printf("%d", card.winning_numbers[i]);
        if (i != card.total_winning_numbers - 1) {
            printf(", ");
        }
    }
    printf("]\n");

    printf("total_playing_numbers: %d\n", card.total_playing_numbers);
    printf("[");
    for (int i = 0; i < card.total_playing_numbers; ++i) {
        printf("%d", card.playing_numbers[i]);
        if (i != card.total_playing_numbers - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}