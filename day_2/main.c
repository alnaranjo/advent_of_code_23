#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **values;
  FILE *fp;
  int count;
} FileContents;

typedef struct {
  int red;
  int green;
  int blue;
} CubeSet;

typedef struct {
  int id;
  int count;
  CubeSet **cubes;
} Game;

typedef struct {
  int id;
  CubeSet cubeset;
} MaxGameData;

typedef struct {
  int game_id;
  int value;
} PowerSet;

typedef enum { red, green, blue } Color;

const size_t BUFFER_MAX_LENGTH = 1024;
const char *DATA_FILENAME = "input.txt";
CubeSet FILTER = {.red = 12, .green = 13, .blue = 14};

int get_file_total_lines(FILE *fp);

FileContents *read_file_contents(const char *filename);
void print_file_contents(FileContents *contents);
void destroy_file_contents(FileContents *contents);

char **split_string(const char *str, const char *delimit, int *total_count);
char *trim_string(char *c);

Game **parse_games(FileContents *contents);
Game *parse_game(const char *str);
void print_game(Game *game);
void destroy_game(Game *game);

CubeSet *parse_cube_set(const char *str);
int calculate_power_set(CubeSet *cubeset);
void print_cube_set(CubeSet *cubeset);

CubeSet get_max_cubeset_values(Game *game);
CubeSet get_min_cubeset_values(Game *game);
MaxGameData **get_max_game_data(Game **games, int count);
MaxGameData **get_min_game_data(Game **games, int count);
MaxGameData **get_max_game_data_filtering(MaxGameData **data, int count,
                                          CubeSet filter, int *total_found);
MaxGameData **get_min_game_data_filtering(MaxGameData **data, int count,
                                          CubeSet filter, int *total_found);
void print_max_game_data(MaxGameData **max_game_data, int count);

int *get_game_ids_filtering(Game **games, int count, CubeSet filter,
                            int *total_found);
int calculate_sum_ids(int *ids, int count);

PowerSet **get_power_set(Game **games, int count, int *total_found);
void print_power_set(PowerSet *set);
int calculate_sum_sets(PowerSet **sets, int count);

int main() {
  printf("Advent of Code 2023 - Day 2\n");
  FileContents *contents = read_file_contents(DATA_FILENAME);
  Game **games = parse_games(contents);
  if (contents == NULL || games == NULL) {
    return 1;
  }

  int count = contents->count;
  int total_ids;
  int *ids = get_game_ids_filtering(games, count, FILTER, &total_ids);

  if (ids == NULL) {
    return 2;
  }

  int total = calculate_sum_ids(ids, total_ids);
  printf("part 1 | total: %d\n", total);

  int total_power_sets = 0;
  PowerSet **sets = get_power_set(games, count, &total_power_sets);
  if (sets == NULL) {
    return 3;
  }

  for (size_t i = 0; i < total_power_sets; ++i) {
    print_power_set(sets[i]);
  }

  int total_sets = calculate_sum_sets(sets, total_power_sets);
  printf("part 2 | total: %d\n", total_sets);

  for (size_t i = 0; i < total_power_sets; ++i) {
    free(sets[i]);
  }
  for (size_t i = 0; i < count; ++i) {
    destroy_game(games[i]);
  }

  free(sets);
  free(games);
  free(ids);

  destroy_file_contents(contents);
  return 0;
}

int get_file_total_lines(FILE *fp) {
  if (fp == NULL) {
    return 0;
  }

  int total_lines = 0;
  char buffer[BUFFER_MAX_LENGTH];
  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    total_lines += 1;
  }

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  return total_lines;
}

FileContents *read_file_contents(const char *filename) {
  if (filename == NULL) {
    return NULL;
  }

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  int total_lines = get_file_total_lines(fp);
  if (total_lines == 0) {
    return NULL;
  }

  int index = 0;
  char **values = malloc(sizeof(char *) * total_lines);

  char buffer[BUFFER_MAX_LENGTH];
  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    char *line = strdup(buffer);
    values[index] = line;
    index += 1;
  }

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  FileContents *result = malloc(sizeof(FileContents));
  result->values = values;
  result->fp = fp;
  result->count = total_lines;

  return result;
}

void print_file_contents(FileContents *contents) {
  if (contents == NULL) {
    return;
  }

  printf("total_lines: %d\n", contents->count);

  for (size_t i = 0; i < contents->count; ++i) {
    char *str = contents->values[i];
    int length = strlen(str);
    printf("[%d] %s", length, str);
  }
}

void destroy_file_contents(FileContents *contents) {
  if (contents == NULL) {
    return;
  }

  if (contents->values != NULL) {
    free(contents->values);
  }

  if (contents->fp != NULL) {
    fclose(contents->fp);
  }

  free(contents);
}

char **split_string(const char *str, const char *delimit, int *total_count) {
  if (str == NULL || delimit == NULL || total_count == NULL) {
    return NULL;
  }

  char *input = strdup(str);

  int total_tokens = 0;
  char **result = malloc(sizeof(char *));
  if (result == NULL) {
    return NULL;
  }

  char *token = trim_string(strtok(input, delimit));
  while (token != NULL) {
    // Make space for new tokenized values
    char **temp = realloc(result, sizeof(char *) * total_tokens + 1);
    if (temp == NULL) {
      free(result);
      return NULL;
    }
    result = temp;

    total_tokens += 1;
    result[total_tokens - 1] = token;

    token = trim_string(strtok(NULL, delimit));
  }

  if (total_tokens > 0) {
    result[total_tokens] = NULL;
    *total_count = total_tokens;
  }

  return result;
}

char *trim_string(char *c) {
  if (c == NULL) {
    return NULL;
  }

  char *e = c + strlen(c) - 1;
  while (*c && isspace(*c))
    c++;
  while (e > c && isspace(*e))
    *e-- = '\0';
  return c;
}

Game **parse_games(FileContents *contents) {
  if (contents == NULL) {
    return NULL;
  }

  Game **games = malloc(sizeof(Game *) * contents->count);
  for (size_t i = 0; i < contents->count; ++i) {
    Game *game = parse_game(contents->values[i]);
    if (game == NULL) {
      free(games);
      return NULL;
    }
    games[i] = game;
  }

  return games;
}

Game *parse_game(const char *str) {
  if (str == NULL) {
    return NULL;
  }

  int total_substrings;
  char **split = split_string(str, ":", &total_substrings);

  if (total_substrings != 2) {
    return NULL;
  }

  char *gameInfo = strdup(split[0]);
  char *cubesInfo = strdup(split[1]);

  split = split_string(gameInfo, " ", &total_substrings);
  if (total_substrings != 2) {
    return NULL;
  }

  int id = atoi(split[1]);

  split = split_string(cubesInfo, ";", &total_substrings);
  if (total_substrings == 0) {
    return NULL;
  }

  int total_cubes = 0;
  CubeSet **cubes = malloc(sizeof(CubeSet *) * total_substrings);

  for (size_t i = 0; i < total_substrings; ++i) {
    CubeSet *cubeset = parse_cube_set(split[i]);
    if (cubeset == NULL) {
      free(cubes);
      return NULL;
    }

    cubes[total_cubes] = cubeset;
    total_cubes += 1;
  }

  Game *game = malloc(sizeof(Game));
  game->id = id;
  game->count = total_cubes;
  game->cubes = cubes;

  return game;
}

void print_game(Game *game) {
  if (game == NULL) {
    return;
  }

  printf("Game %d, total: %d\n", game->id, game->count);
  if (game->cubes == NULL) {
    return;
  }

  for (size_t i = 0; i < game->count; ++i) {
    print_cube_set(game->cubes[i]);
  }
}

void destroy_game(Game *game) {
  if (game == NULL) {
    return;
  }

  if (game->cubes != NULL) {
    for (size_t i = 0; i < game->count; ++i) {
      free(game->cubes[i]);
    }
  }

  free(game);
}

CubeSet *parse_cube_set(const char *str) {
  if (str == NULL) {
    return NULL;
  }

  int total_substrings = 0;
  char **split = split_string(str, ",", &total_substrings);
  if (split == NULL) {
    return NULL;
  }

  if (total_substrings == 0) {
    return NULL;
  }

  CubeSet *cubeset = malloc(sizeof(CubeSet));
  for (size_t i = 0; i < total_substrings; ++i) {
    int total_colors = 0;
    char **color_info = split_string(split[i], " ", &total_colors);
    if (total_colors == 0) {
      free(cubeset);
      return NULL;
    }

    int value = atoi(color_info[0]);
    char *color = color_info[1];

    if (strcmp(color, "red") == 0) {
      cubeset->red = value;
    } else if (strcmp(color, "green") == 0) {
      cubeset->green = value;
    } else if (strcmp(color, "blue") == 0) {
      cubeset->blue = value;
    }
  }

  return cubeset;
}

int calculate_power_set(CubeSet *cubeset) {
  if (cubeset == NULL) {
    return 0;
  }

  return cubeset->red * cubeset->green * cubeset->blue;
}

void print_cube_set(CubeSet *cubeset) {
  if (cubeset == NULL) {
    return;
  }
  printf("red: %d, green: %d, blue: %d\n", cubeset->red, cubeset->green,
         cubeset->blue);
}

CubeSet get_max_cubeset_values(Game *game) {
  if (game == NULL || game->count == 0) {
    CubeSet cubeset;
    return cubeset;
  }

  CubeSet max = {.red = 0, .green = 0, .blue = 0};
  for (size_t i = 0; i < game->count; ++i) {
    CubeSet current = *game->cubes[i];
    if (current.red > max.red) {
      max.red = current.red;
    }
    if (current.green > max.green) {
      max.green = current.green;
    }
    if (current.blue > max.blue) {
      max.blue = current.blue;
    }
  }

  return max;
}

CubeSet get_min_cubeset_values(Game *game) {
  if (game == NULL || game->count == 0) {
    CubeSet cubeset;
    return cubeset;
  }

  CubeSet max = {.red = 0, .green = 0, .blue = 0};
  for (size_t i = 0; i < game->count; ++i) {
    CubeSet current = *game->cubes[i];
    if (current.red < max.red) {
      max.red = current.red;
    }
    if (current.green < max.green) {
      max.green = current.green;
    }
    if (current.blue < max.blue) {
      max.blue = current.blue;
    }
  }

  return max;
}

MaxGameData **get_max_game_data(Game **games, int count) {
  if (games == NULL || count < 0) {
    return NULL;
  }

  int total = 0;

  // Reserve enough space for entire list
  MaxGameData **result = malloc(sizeof(MaxGameData *) * count);

  for (size_t i = 0; i < count; ++i) {
    CubeSet cubeset = get_max_cubeset_values(games[i]);
    MaxGameData *res = malloc(sizeof(MaxGameData));
    res->id = games[i]->id;
    res->cubeset = cubeset;
    result[i] = res;
    total += 1;
  }

  // Resize to fit only found values
  MaxGameData **temp = realloc(result, sizeof(MaxGameData *) * total + 1);
  if (temp == NULL) {
    free(result);
    return NULL;
  }

  result = temp;

  return result;
}

MaxGameData **get_min_game_data(Game **games, int count) {
  if (games == NULL || count < 0) {
    return NULL;
  }

  int total = 0;

  // Reserve enough space for entire list
  MaxGameData **result = malloc(sizeof(MaxGameData *) * count);

  for (size_t i = 0; i < count; ++i) {
    CubeSet cubeset = get_min_cubeset_values(games[i]);
    MaxGameData *res = malloc(sizeof(MaxGameData));
    res->id = games[i]->id;
    res->cubeset = cubeset;
    result[i] = res;
    total += 1;
  }

  // Resize to fit only found values
  MaxGameData **temp = realloc(result, sizeof(MaxGameData *) * total + 1);
  if (temp == NULL) {
    free(result);
    return NULL;
  }

  result = temp;

  return result;
}

MaxGameData **get_max_game_data_filtering(MaxGameData **data, int count,
                                          CubeSet filter, int *total_fount) {
  if (data == NULL) {
    return NULL;
  }

  int total = 0;

  // Reserve enough space for entire list
  MaxGameData **result = malloc(sizeof(MaxGameData *) * count);
  for (size_t i = 0; i < count; ++i) {
    MaxGameData *current = data[i];
    if (filter.red >= current->cubeset.red &&
        filter.green >= current->cubeset.green &&
        filter.blue >= current->cubeset.blue) {
      MaxGameData *new = malloc(sizeof(MaxGameData));
      new->id = data[i]->id;
      new->cubeset = data[i]->cubeset;
      result[total] = new;
      total += 1;
    }
  }

  // Resize to fit only found values
  MaxGameData **temp = realloc(result, sizeof(MaxGameData *) * total + 1);
  if (temp == NULL) {
    free(result);
    return NULL;
  }

  result = temp;
  *total_fount = total;

  return result;
}

MaxGameData **get_min_game_data_filtering(MaxGameData **data, int count,
                                          CubeSet filter, int *total_fount) {
  if (data == NULL) {
    return NULL;
  }

  int total = 0;

  // Reserve enough space for entire list
  MaxGameData **result = malloc(sizeof(MaxGameData *) * count);
  for (size_t i = 0; i < count; ++i) {
    MaxGameData *current = data[i];
    if (filter.red <= current->cubeset.red &&
        filter.green <= current->cubeset.green &&
        filter.blue <= current->cubeset.blue) {
      MaxGameData *new = malloc(sizeof(MaxGameData));
      new->id = data[i]->id;
      new->cubeset = data[i]->cubeset;
      result[total] = new;
      total += 1;
    }
  }

  // Resize to fit only found values
  MaxGameData **temp = realloc(result, sizeof(MaxGameData *) * total + 1);
  if (temp == NULL) {
    free(result);
    return NULL;
  }

  result = temp;
  *total_fount = total;

  return result;
}

void print_max_game_data(MaxGameData **max_game_data, int count) {
  if (max_game_data == NULL) {
    return;
  }

  printf("total: %d\n", count);
  for (size_t i = 0; i < count; ++i) {
    printf("id: %d | ", max_game_data[i]->id);
    print_cube_set(&max_game_data[i]->cubeset);
  }
}

int *get_game_ids_filtering(Game **games, int count, CubeSet filter,
                            int *total_found) {
  if (games == NULL || count < 0) {
    return NULL;
  }

  int total_filtered;
  MaxGameData **all_max_data = get_max_game_data(games, count);
  MaxGameData **filtered_max_data =
      get_max_game_data_filtering(all_max_data, count, filter, &total_filtered);

  if (all_max_data == NULL || filtered_max_data == NULL) {
    return NULL;
  }

  int *result = malloc(sizeof(int) * total_filtered);
  for (size_t i = 0; i < total_filtered; ++i) {
    result[i] = filtered_max_data[i]->id;
  }
  *total_found = total_filtered;

  for (size_t i = 0; i < count; ++i) {
    free(all_max_data[i]);
  }
  free(all_max_data);

  for (size_t i = 0; i < total_filtered; ++i) {
    free(filtered_max_data[i]);
  }
  free(filtered_max_data);

  return result;
}

int calculate_sum_ids(int *ids, int count) {
  if (ids == NULL) {
    return 0;
  }

  int total = 0;
  for (size_t i = 0; i < count; ++i) {
    total += ids[i];
  }

  return total;
}

PowerSet **get_power_set(Game **games, int count, int *total_found) {
  if (games == NULL || count < 0) {
    return NULL;
  }

  MaxGameData **all_max_data = get_max_game_data(games, count);
  if (all_max_data == NULL) {
    return NULL;
  }

  PowerSet **result = malloc(sizeof(PowerSet *) * count);
  for (size_t i = 0; i < count; ++i) {
    int value = calculate_power_set(&all_max_data[i]->cubeset);

    PowerSet *set = malloc(sizeof(PowerSet));
    set->game_id = all_max_data[i]->id;
    set->value = value;
    result[i] = set;
  }

  *total_found = count;
  return result;
}

void print_power_set(PowerSet *set) {
  if (set == NULL) {
    return;
  }

  printf("id: %d, value: %d\n", set->game_id, set->value);
}

int calculate_sum_sets(PowerSet **sets, int count) {
  if (sets == NULL) {
    return 0;
  }

  int total = 0;
  for (size_t i = 0; i < count; ++i) {
    total += sets[i]->value;
  }

  return total;
}