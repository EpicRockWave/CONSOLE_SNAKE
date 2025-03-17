#include <algorithm>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#define MAP_WIDTH 20
#define MAP_HEIGHT 20
#define PIXEL_COUNT (MAP_HEIGHT * MAP_WIDTH)
#define TICK 250
#define APPLE_COUNT 3

#define COLORIZE(text, col_code) "\033[" #col_code "m" #text "\033[0m"

struct node_list {
  std::string SNAKE = COLORIZE(o, 32);
  std::string WALL = COLORIZE(_, 47);
  std::string APPLE = COLORIZE(@, 31);
  std::string BLANK = " ";
};

struct pos {
  int x = 0, y = 0;

  pos operator+=(const pos &pos2) {
    x += pos2.x;
    y += pos2.y;
    return *this;
  }

  bool operator==(const pos &pos2) const {
    return (x == pos2.x) && (y == pos2.y);
  }

  pos operator+(const pos &pos2) const { return {x + pos2.x, y + pos2.y}; }
};

struct dir {
  pos UP = {0, -1}, DOWN = {0, 1}, RIGHT = {1, 0}, LEFT = {-1, 0},
      ZERO = {0, 0};
};

std::string *screen = nullptr;
std::vector<pos> apples;
node_list nodes;
dir dirs;

void init_map();
void render();
void process_input(pos &dir);
void color_pixel(const pos &xy, const std::string &ch);

void apple_logic(const std::vector<pos> &snake, int &snake_len);
void spawn_apple(const std::vector<pos> &snake);
bool detect_collision(const std::vector<pos> &snake);

int main() {
  system("title Snake Game");
  std::vector<pos> snake;
  pos head = {10, 10}, dir = dirs.UP;
  int snake_len = 2;

  srand(time(0)); /// seed
  init_map();

  for (int i = 0; i < APPLE_COUNT; i++)
    spawn_apple(snake);

  while (true) {
    if (_kbhit())
      process_input(dir); /// controls

    /// SNAKE LOGIC ///
    head += dir;
    color_pixel(head, nodes.SNAKE);
    snake.insert(snake.begin(), head);

    if (snake.size() > snake_len) {
      color_pixel(snake.back(), nodes.BLANK);
      snake.pop_back();
    }
    /////////////////////////////////

    /// PROCESS GAME OVER ///
    if (detect_collision(snake)) {
      std::cout << COLORIZE(GAME OVER.PRESS ANY KEY TO CONTINUE..., 31)
                << std::endl;
      _getch();
      break;
    }
    ///////////////////////////

    apple_logic(snake, snake_len);

    render();
    Sleep(TICK);
  }

  delete[] screen;
  return 0;
}

void init_map() {
  screen = new std::string[PIXEL_COUNT];
  for (int i = 0; i < PIXEL_COUNT; i++) {
    int x = i % MAP_WIDTH, y = i / MAP_WIDTH;
    screen[i] = (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1)
                    ? nodes.WALL
                    : nodes.BLANK;
  }
}

void process_input(pos &dir) {
  pos newdir;
  char inp = _getch();
  switch (inp) {
  case 'w':
  case 'W':
    newdir = dirs.UP;
    break;
  case 's':
  case 'S':
    newdir = dirs.DOWN;
    break;
  case 'a':
  case 'A':
    newdir = dirs.LEFT;
    break;
  case 'd':
  case 'D':
    newdir = dirs.RIGHT;
    break;
  }
  if (newdir + dir != dirs.ZERO)
    dir = newdir;
}

void gotoxy(int x, int y) {
  COORD coord = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void render() {
  gotoxy(0, 0); /// move cursor to start to render a new frame
  for (int i = 0; i < MAP_WIDTH; i++) {
    for (int j = 0; j < MAP_HEIGHT; j++) {
      std::cout << screen[i * MAP_WIDTH + j];
    }
    std::cout << std::endl;
  }
}

void color_pixel(const pos &xy, const std::string &ch) {
  screen[xy.y * MAP_WIDTH + xy.x] = ch;
}

bool detect_collision(const std::vector<pos> &snake) {
  pos head_pos = snake.front();
  if (head_pos.x >= MAP_WIDTH - 1 || head_pos.x < 1 ||
      head_pos.y >= MAP_HEIGHT - 1 || head_pos.y < 1) /// collision with borders
    return true;

  if (std::find(snake.begin() + 1, snake.end(), head_pos) !=
      snake.end()) /// collision with body
    return true;

  return false;
}

void spawn_apple(const std::vector<pos> &snake) {
  pos apple_pos;
  do {
    apple_pos = {
        (rand() % (MAP_HEIGHT - 2) + 1), // min + rand() % (max - min + 1)
        (rand() % (MAP_WIDTH - 2) + 1)};
  } while (std::find(apples.begin(), apples.end(), apple_pos) != apples.end() ||
           std::find(snake.begin(), snake.end(), apple_pos) !=
               snake.end()); /// basicaly its needed to not spawn apple inside
                             /// of snake or another apple

  apples.push_back(apple_pos);
  color_pixel(apple_pos, nodes.APPLE);
}

void apple_logic(const std::vector<pos> &snake, int &snake_len) {
  auto it = std::find(apples.begin(), apples.end(), snake.front());
  if (it != apples.end()) {
    snake_len++;
    apples.erase(it);
    spawn_apple(snake);
  }
}
