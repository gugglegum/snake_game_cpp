#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <vector>

#define FIELD_CELL_TYPE_NONE 0
#define FIELD_CELL_TYPE_APPLE -1
#define SNAKE_DIRECTION_UP 0
#define SNAKE_DIRECTION_RIGHT 1
#define SNAKE_DIRECTION_DOWN 2
#define SNAKE_DIRECTION_LEFT 3

const int field_size_x = 35;
const int field_size_y = 25;
const int cell_size = 32;
const int window_width = field_size_x * cell_size;
const int window_height = field_size_y * cell_size;

int field[field_size_y][field_size_x];
int snake_position_x = field_size_x / 2;
int snake_position_y = field_size_y / 2;
int snake_length = 4;
int snake_direction = SNAKE_DIRECTION_RIGHT;
bool game_over = false;

sf::Texture snake_texture;
sf::Sprite snake;

sf::Texture none_texture;
sf::Sprite none;

sf::Texture apple_texture;
sf::Sprite apple;

void init_game()
{
    srand(time(NULL));

    snake_texture.loadFromFile("images/snake.png");
    snake.setTexture(snake_texture);

    none_texture.loadFromFile("images/none.png");
    none.setTexture(none_texture);

    apple_texture.loadFromFile("images/apple.png");
    apple.setTexture(apple_texture);
}

int get_random_empty_cell()
{
    int empty_cell_count = 0;
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] == FIELD_CELL_TYPE_NONE) {
                empty_cell_count++;
            }
        }
    }
    int target_empty_cell_index = rand() % empty_cell_count;
    int empty_cell_index = 0;
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] == FIELD_CELL_TYPE_NONE) {
                if (empty_cell_index == target_empty_cell_index) {
                    return j * field_size_x + i;
                }
                empty_cell_index++;
            }
        }
    }
    return -1;
}

void add_apple()
{
    int apple_pos = get_random_empty_cell();
    if (apple_pos != -1) {
        field[apple_pos / field_size_x][apple_pos % field_size_x] = FIELD_CELL_TYPE_APPLE;
    }
}

void clear_field()
{
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            field[j][i] = FIELD_CELL_TYPE_NONE;
        }
    }
    for (int i = 0; i < snake_length; i++) {
        field[snake_position_y][snake_position_x - i] = snake_length - i;
    }
    add_apple();
}

void draw_field(sf::RenderWindow &window)
{
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            switch (field[j][i]) {
                case FIELD_CELL_TYPE_NONE:
                    none.setPosition(float(i * cell_size), float(j * cell_size));
                    window.draw(none);
                    break;
                case FIELD_CELL_TYPE_APPLE:
                    apple.setPosition(float(i * cell_size), float(j * cell_size));
                    window.draw(apple);
                    break;
                default:
                    snake.setPosition(float(i * cell_size), float(j * cell_size));
                    window.draw(snake);
            }
        }
    }
}

void grow_snake()
{
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] > FIELD_CELL_TYPE_NONE) {
                field[j][i]++;
            }
        }
    }
}

void make_move()
{
    switch (snake_direction) {
        case SNAKE_DIRECTION_UP:
            snake_position_y--;
            if (snake_position_y < 0) {
                snake_position_y = field_size_y - 1;
            }
            break;
        case SNAKE_DIRECTION_RIGHT:
            snake_position_x++;
            if (snake_position_x > field_size_x - 1) {
                snake_position_x = 0;
            }
            break;
        case SNAKE_DIRECTION_DOWN:
            snake_position_y++;
            if (snake_position_y > field_size_y - 1) {
                snake_position_y = 0;
            }
            break;
        case SNAKE_DIRECTION_LEFT:
            snake_position_x--;
            if (snake_position_x < 0) {
                snake_position_x = field_size_x - 1;
            }
            break;
    }

    if (field[snake_position_y][snake_position_x] != FIELD_CELL_TYPE_NONE) {
        switch (field[snake_position_y][snake_position_x]) {
            case FIELD_CELL_TYPE_APPLE:
                snake_length++;
                grow_snake();
                add_apple();
                break;
            default:
                game_over = true;
        }
    }

    field[snake_position_y][snake_position_x] = snake_length + 1;

    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] > FIELD_CELL_TYPE_NONE) {
                field[j][i]--;
            }
        }
    }
}

int main()
{
    init_game();

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Snake", sf::Style::Close);

    clear_field();

    std::vector<int> snake_direction_queue;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                int snake_direction_last = snake_direction_queue.empty() ? snake_direction : snake_direction_queue.at(0);
                switch (event.key.code) {
                    case sf::Keyboard::Up:
                        if (snake_direction_last != SNAKE_DIRECTION_DOWN) {
                            if (snake_direction_queue.size() < 2) {
                                snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_UP);
                            }
                        }
                        break;
                    case sf::Keyboard::Right:
                        if (snake_direction_last != SNAKE_DIRECTION_LEFT) {
                            if (snake_direction_queue.size() < 2) {
                                snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_RIGHT);
                            }
                        }
                        break;
                    case sf::Keyboard::Down:
                        if (snake_direction_last != SNAKE_DIRECTION_UP) {
                            if (snake_direction_queue.size() < 2) {
                                snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_DOWN);
                            }
                        }
                        break;
                    case sf::Keyboard::Left:
                        if (snake_direction_last != SNAKE_DIRECTION_RIGHT) {
                            if (snake_direction_queue.size() < 2) {
                                snake_direction_queue.insert(snake_direction_queue.begin(), SNAKE_DIRECTION_LEFT);
                            }
                        }
                        break;
                    case sf::Keyboard::Escape:
                        game_over = true;
                        break;
                }
            }
        }
        if (!snake_direction_queue.empty()) {
            snake_direction = snake_direction_queue.back();
            snake_direction_queue.pop_back();
        }

        make_move();

        if (game_over) {
            window.close();
        }

        window.clear(sf::Color(183, 212, 168));

        draw_field(window);

        window.display();

        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}
