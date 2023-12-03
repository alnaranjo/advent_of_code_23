#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t BUFFER_MAX_LENGTH = 1024;
const char *DATA_FILENAME = "text.txt";

typedef struct {
  char **values;
  FILE *fp;
  int count;
} FileContents;

int get_file_total_lines(FILE *fp);

FileContents *read_file_contents(const char *filename);
void print_file_contents(FileContents *contents);
void destroy_file_contents(FileContents *contents);

char **split_string(const char *str, const char *delimit, int *total_count);
char *trim_string(char *c);

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

Game **parse_games(FileContents *contents);
Game *parse_game(const char *str);
void print_game(Game *game);
void destroy_game(Game *game);

CubeSet *parse_cube_set(const char *str);
void print_cube_set(CubeSet *cubeset);

int main() {
  printf("Advent of Code 2023 - Day 2\n");
  FileContents *contents = read_file_contents(DATA_FILENAME);
  print_file_contents(contents);

  Game **games = parse_games(contents);
  for (size_t i = 0; i < contents->count; ++i) {
    print_game(games[i]);
  }

  for (size_t i = 0; i < contents->count; ++i) {
    destroy_game(games[i]);
  }
  free(games);

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

void print_cube_set(CubeSet *cubeset) {
  if (cubeset == NULL) {
    return;
  }
  printf("red: %d, green: %d, blue: %d\n", cubeset->red, cubeset->green,
         cubeset->blue);
}