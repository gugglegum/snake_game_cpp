#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <vector>

#define FIELD_CELL_TYPE_NONE 0
#define FIELD_CELL_TYPE_APPLE -1
#define FIELD_CELL_TYPE_WALL -2
#define SNAKE_DIRECTION_UP 0
#define SNAKE_DIRECTION_RIGHT 1
#define SNAKE_DIRECTION_DOWN 2
#define SNAKE_DIRECTION_LEFT 3
#define MENU_ITEM_START "Start new game"
#define MENU_ITEM_SETTINGS "Settings"
#define MENU_ITEM_QUIT "Quit"

const int field_size_x = 35;
const int field_size_y = 25;
const int cell_size = 32;
const int score_bar_height = 60;
const int window_width = field_size_x * cell_size;
const int window_height = field_size_y * cell_size + score_bar_height;

int field[field_size_y][field_size_x];
int snake_position_x;
int snake_position_y;
int snake_length = 0;
int snake_direction = SNAKE_DIRECTION_RIGHT;
int score = 0;
bool game_started = false;
bool game_over = false;
int game_over_timeout = 0;
bool game_paused = true;
int current_menu_item_index = 0;

sf::Texture snake_texture;
sf::Sprite snake;

sf::Texture none_texture;
sf::Sprite none;

sf::Texture apple_texture;
sf::Sprite apple;

sf::Texture wall_texture;
sf::Sprite wall;

sf::SoundBuffer sb_menu_navigate;
sf::Sound sound_menu_navigate;

sf::SoundBuffer sb_game_start;
sf::Sound sound_game_start;

sf::SoundBuffer sb_ate_apple;
sf::Sound sound_ate_apple;

sf::SoundBuffer sb_died_against_the_wall;
sf::Sound sound_died_against_the_wall;

sf::SoundBuffer sb_ate_himself;
sf::Sound sound_ate_himself;

sf::Font font_score;
sf::Text text_score;

sf::Font font_title;
sf::Text text_title;

sf::Font font_game_over;
sf::Text text_game_over;

sf::Font font_menu;
std::vector<sf::Text> text_menu_items;
std::vector<std::string> menu_items = {MENU_ITEM_START, MENU_ITEM_SETTINGS, MENU_ITEM_QUIT};

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
    int target_empty_cell_index = std::rand() % empty_cell_count;
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
    for (int i = 0; i < field_size_x; i++) {
        if (i < 5 || field_size_x - i - 1 < 5) {
            field[0][i] = FIELD_CELL_TYPE_WALL;
            field[field_size_y - 1][i] = FIELD_CELL_TYPE_WALL;
        }
    }
    for (int i = 1; i < field_size_y - 1; i++) {
        if (i < 5 || field_size_y - i - 1 < 5) {
            field[i][0] = FIELD_CELL_TYPE_WALL;
            field[i][field_size_x - 1] = FIELD_CELL_TYPE_WALL;
        }
    }
    add_apple();
}

void init_game()
{
    std::srand(time(NULL));
    clear_field();

    snake_texture.loadFromFile("images/snake.png");
    snake.setTexture(snake_texture);

    none_texture.loadFromFile("images/none.png");
    none.setTexture(none_texture);

    apple_texture.loadFromFile("images/apple.png");
    apple.setTexture(apple_texture);

    wall_texture.loadFromFile("images/wall.png");
    wall.setTexture(wall_texture);

    sb_menu_navigate.loadFromFile("sounds/menu-navigate-02.wav");
    sound_menu_navigate.setBuffer(sb_menu_navigate);

    sb_game_start.loadFromFile("sounds/jingle-achievement-01.wav");
    sound_game_start.setBuffer(sb_game_start);

    sb_ate_apple.loadFromFile("sounds/collect-point-01.wav");
    sound_ate_apple.setBuffer(sb_ate_apple);

    sb_died_against_the_wall.loadFromFile("sounds/explosion-02.wav");
    sound_died_against_the_wall.setBuffer(sb_died_against_the_wall);

    sb_ate_himself.loadFromFile("sounds/explosion-00.wav");
    sound_ate_himself.setBuffer(sb_ate_himself);

    font_score.loadFromFile("fonts/ShockMintFund-YzA8v.ttf");
    text_score.setFont(font_score);
    text_score.setCharacterSize(36);
    text_score.setFillColor(sf::Color::Black);

    font_title.loadFromFile("fonts/BigfatScript-2OvA8.otf");
    text_title.setFont(font_title);
    text_title.setString("Snake");
    text_title.setCharacterSize(40);
    text_title.setFillColor(sf::Color::Black);
    text_title.setPosition(20, 0);

    font_game_over.loadFromFile("fonts/BigOldBoldy-dEjR.ttf");
    text_game_over.setFont(font_game_over);
    text_game_over.setString("GAME OVER");
    text_game_over.setCharacterSize(120);
    text_game_over.setFillColor(sf::Color::Red);
    text_game_over.setPosition((window_width - text_game_over.getLocalBounds().width) / 2, (window_height - text_game_over.getLocalBounds().height + score_bar_height) / 2);

    font_menu.loadFromFile("fonts/BigOldBoldy-dEjR.ttf");
    for (int i = 0; i < menu_items.size(); i++) {
        text_menu_items.emplace_back(sf::Text());
        text_menu_items.back().setString(menu_items.at(i));
        text_menu_items.back().setFont(font_menu);
        text_menu_items.back().setCharacterSize(40);
    }
}

void start_game()
{
    snake_position_x = field_size_x / 2;
    snake_position_y = field_size_y / 2;
    snake_length = 4;
    snake_direction = SNAKE_DIRECTION_RIGHT;
    score = 0;
    game_started = true;
    game_over = false;
    game_paused = false;
    clear_field();
    sound_game_start.play();
}

void finish_game()
{
    game_over = true;
    game_paused = true;
    game_over_timeout = 20;
    current_menu_item_index = 0;
}

void draw_field(sf::RenderWindow &window)
{
    for (int j = 0; j < field_size_y; j++) {
        for (int i = 0; i < field_size_x; i++) {
            switch (field[j][i]) {
                case FIELD_CELL_TYPE_NONE:
                    none.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(none);
                    break;
                case FIELD_CELL_TYPE_APPLE:
                    apple.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(apple);
                    break;
                case FIELD_CELL_TYPE_WALL:
                    wall.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(wall);
                    break;
                default:
                    snake.setPosition(float(i * cell_size), float(j * cell_size + score_bar_height));
                    window.draw(snake);
            }
        }
    }
}

void draw_score_bar(sf::RenderWindow &window)
{
    window.draw(text_title);

    text_score.setString("Score: " + std::to_string(score));
    text_score.setPosition(window_width - text_score.getLocalBounds().width - 20, 10);
    window.draw(text_score);
}

void draw_menu(sf::RenderWindow &window)
{
    float const menu_padding_horizontal = 40;
    float const menu_padding_vertical = 30;
    float const menu_item_interval = 20;

    float menu_item_max_width = 0;
    float current_menu_item_offset_y = 0;
    for (int i = 0; i < text_menu_items.size(); i++) {
        text_menu_items.at(i).setPosition(0, current_menu_item_offset_y);
        text_menu_items.at(i).setFillColor(current_menu_item_index == i ? sf::Color(224, 224, 224) : sf::Color(128, 128, 128));
        current_menu_item_offset_y += text_menu_items.at(i).getLocalBounds().height + menu_item_interval;
        menu_item_max_width = std::max(menu_item_max_width, text_menu_items.at(i).getLocalBounds().width);
    }

    float const menu_width = menu_item_max_width + menu_padding_horizontal * 2;
    float const menu_height = current_menu_item_offset_y - menu_item_interval + menu_padding_vertical * 2;

    float const menu_position_x = (window_width - menu_width) / 2;
    float const menu_position_y = (window_height - menu_height) / 2;

    sf::RectangleShape menu_rect(sf::Vector2f(menu_width, menu_height));
    menu_rect.setPosition(menu_position_x, menu_position_y);
    menu_rect.setFillColor(sf::Color(0, 0, 0, 224));
    window.draw(menu_rect);

    for (int i = 0; i < text_menu_items.size(); i++) {
        text_menu_items.at(i).move(menu_position_x + menu_padding_horizontal, menu_position_y + menu_padding_vertical);
        window.draw(text_menu_items.at(i));
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
                sound_ate_apple.play();
                snake_length++;
                score++;
                grow_snake();
                add_apple();
                break;
            case FIELD_CELL_TYPE_WALL:
                sound_died_against_the_wall.play();
                finish_game();
                break;
            default:
                if (field[snake_position_y][snake_position_x] > 1) {
                    sound_ate_himself.play();
                    finish_game();
                }
        }
    }

    if (!game_over) {
        for (int j = 0; j < field_size_y; j++) {
            for (int i = 0; i < field_size_x; i++) {
                if (field[j][i] > FIELD_CELL_TYPE_NONE) {
                    field[j][i]--;
                }
            }
        }
        field[snake_position_y][snake_position_x] = snake_length;
    }
}

int main()
{
    init_game();

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Snake", sf::Style::Close);

    std::vector<int> snake_direction_queue;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (game_paused) {
                    if (game_over_timeout == 0) {
                        switch (event.key.code) {
                            case sf::Keyboard::Up:
                                sound_menu_navigate.play();
                                current_menu_item_index--;
                                if (current_menu_item_index < 0) {
                                    current_menu_item_index = text_menu_items.size() - 1;
                                }
                                break;
                            case sf::Keyboard::Down:
                                sound_menu_navigate.play();
                                current_menu_item_index++;
                                if (current_menu_item_index > text_menu_items.size() - 1) {
                                    current_menu_item_index = 0;
                                }
                                break;
                            case sf::Keyboard::Enter:
                                if (menu_items.at(current_menu_item_index) == MENU_ITEM_START) {
                                    start_game();
                                }
                                if (menu_items.at(current_menu_item_index) == MENU_ITEM_QUIT) {
                                    window.close();
                                }
                                break;
                            case sf::Keyboard::Escape:
                                if (!game_over && game_started) {
                                    game_paused = false;
                                }
                                break;
                        }
                    } else {
                        game_over_timeout = 0;
                    }
                } else {
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
                            game_paused = true;
                            break;
                    }
                }
            }
        }
        if (!snake_direction_queue.empty()) {
            snake_direction = snake_direction_queue.back();
            snake_direction_queue.pop_back();
        }

        if (!game_paused) {
            make_move();
        }

        window.clear(sf::Color(183, 212, 168));

        draw_field(window);
        draw_score_bar(window);

        if (game_over) {
            window.draw(text_game_over);
            if (game_over_timeout > 0) {
                game_over_timeout--;
            }
        }

        if (game_paused && game_over_timeout == 0) {
            draw_menu(window);
        }

        window.display();

        sf::sleep(sf::milliseconds(100));
    }

    return 0;
}
