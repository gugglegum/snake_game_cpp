#include <SFML/Graphics.hpp>
#include <stdlib.h>

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

int GetRandomEmptyCell()
{
    int emptyCellCount = 0;
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] == FIELD_CELL_TYPE_NONE) {
                emptyCellCount++;
            }
        }
    }
    int targetEmptyCellIndex = rand() % emptyCellCount;
    int emptyCellIndex = 0;
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            if (field[j][i] == FIELD_CELL_TYPE_NONE) {
                if (emptyCellIndex == targetEmptyCellIndex) {
                    return j * field_size_x + i;
                }
                emptyCellIndex++;
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
    sf::Texture snake_texture;
    snake_texture.loadFromFile("images/snake.png");
    sf::Sprite snake;
    snake.setTexture(snake_texture);

    sf::Texture none_texture;
    none_texture.loadFromFile("images/none.png");
    sf::Sprite none;
    none.setTexture(none_texture);

    sf::Texture apple_texture;
    apple_texture.loadFromFile("images/apple.png");
    sf::Sprite apple;
    apple.setTexture(apple_texture);

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

void handle_keyboard()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        if (snake_direction != SNAKE_DIRECTION_DOWN) {
            snake_direction = SNAKE_DIRECTION_UP;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (snake_direction != SNAKE_DIRECTION_LEFT) {
            snake_direction = SNAKE_DIRECTION_RIGHT;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (snake_direction != SNAKE_DIRECTION_UP) {
            snake_direction = SNAKE_DIRECTION_DOWN;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (snake_direction != SNAKE_DIRECTION_RIGHT) {
            snake_direction = SNAKE_DIRECTION_LEFT;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        game_over = true;
    }
}

int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Snake", sf::Style::Close);

    clear_field();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        make_move();

        if (game_over) {
            window.close();
        }

        window.clear(sf::Color(183, 212, 168));

        draw_field(window);

        handle_keyboard();

        window.display();

        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}
