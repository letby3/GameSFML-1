#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <iostream> 
#include <windows.h>   // для SetConsoleCP()
#include <math.h>
using namespace std;
using namespace sf;
#define RUS 1251
#define texture_side 32 //размеры одного квадратика текстуры 32x32 пикселя
#define num_sq 12 //количество квадратиков в текстуре
#define win 1
#define lose -1
#define open_cell 1
#define close_cell 0
#define flag_cell 3
#define width_win_lose 108
#define height_win_lose 34
#define dop_width_win 200
#define pos_x_win_lose 46
#define pos_y_win_lose 40

int arr_check[625]; //массив максимального размера, содержащий роли каждого квадратика (автоматичеки содержит нули)
int arr_check_open[625]; // массив, содержащий информацию о том, открыта ли клетка или нет
const int empty_sq = 0; //номер пустого квадратика
//номера спрайтов, содержащих цифры
const int one = 1;
const int two = 2;
const int three = 3;
const int four = 4;
const int five = 5;
const int six = 6;
const int seven = 7;
const int eight = 8;
const int sp_mine = 9; //номер спрайта бомбы
const int sq = 10; //номер объёмного квадратика
const int flag = 11; //номер спрайта флажка

//переменные для размеров игрового поля и кол-ва бомб
const int width_beginner = 9; const int height_beginner = 9; const int mine_beginner = 10; //новичок
const int width_amateur = 16; const int height_amateur = 16; const int mine_amateur = 40; //любитель
const int width_professional = 25; const int height_professional = 25; const int mine_professional = 99; //профессионал

void mine(int width_sq, int height_sq, int num_mine) { //функция закладывания бомб и подсчёта рядом лежащих бомб
	int cnt_mine = 0;
	int position_mine;
	while (cnt_mine < num_mine) {
		position_mine = rand() % (width_sq * height_sq) + 1;
		if (arr_check[position_mine] != sp_mine) { 
			arr_check[position_mine] = sp_mine; //закладываем бомбу
			cnt_mine++; //увеличиваем кол-во заложенных бомб
			
		}
	}

	int curr_cnt_mine = 0; //переменная кол-ва рядом лежащих бомб 
	for (int i = 0; i < width_sq * height_sq; i++) { //определяем, какое кол-во бомб лежит рядом с той или иной ячейкой
		if (arr_check[i] != sp_mine) {
			if (i - width_sq >= 0) { //верхняя полоса
				if (arr_check[i - width_sq] == sp_mine) curr_cnt_mine++;
				if (i - width_sq - 1 > 0 and arr_check[i - width_sq - 1] == sp_mine and (i - width_sq - 1) % width_sq != width_sq-1) curr_cnt_mine++;
				if (i - width_sq + 1 < width_sq*height_sq and arr_check[i - width_sq + 1] == sp_mine and (i - width_sq + 1) % width_sq != 0) curr_cnt_mine++;
			}
			if (i + width_sq < width_sq*height_sq) { //нижняя полоса
				if (arr_check[i + width_sq] == sp_mine) curr_cnt_mine++;
				if (i + width_sq - 1 > 0 and arr_check[i + width_sq - 1] == sp_mine and (i + width_sq - 1)%width_sq !=  width_sq - 1) curr_cnt_mine++;
				if (i + width_sq + 1 < width_sq * height_sq and arr_check[i + width_sq + 1] == sp_mine and (i + width_sq + 1)%width_sq != 0) curr_cnt_mine++;
			}
			if (i - 1 > 0 and (i - 1)%width_sq != width_sq - 1) { //левая полоса
				if (arr_check[i - 1] == sp_mine) curr_cnt_mine++;
			}
			if (i + 1 < width_sq * height_sq and (i + 1)%width_sq != 0) { //правая полоса
				if (arr_check[i + 1] == sp_mine) curr_cnt_mine++;
			}
			arr_check[i] = curr_cnt_mine;
			curr_cnt_mine = 0; 
		}
	}
}

void empty_cell(int width_sq, int height_sq, int index) { //функция создания пустой клетки 
	arr_check_open[index] = open_cell;
	if (index - width_sq >= 0 and arr_check[index] == empty_sq) { //верхняя полоса
		if (arr_check[index - width_sq] != sp_mine and arr_check_open[index - width_sq] == close_cell) { //если не бомба и не открыта
			if (arr_check[index - width_sq] >= one and arr_check[index - width_sq] <= eight) //если число 
				arr_check_open[index - width_sq] = open_cell;
			else {
				if (arr_check[index - width_sq] == empty_sq and arr_check_open[index - width_sq] == close_cell) //если пустая клетка и она ещё не открыта
					arr_check_open[index - width_sq] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index - width_sq);
			}
		}
		if (index - width_sq - 1 > 0 and (index - width_sq - 1) % width_sq != width_sq - 1 and arr_check[index - width_sq - 1] != sp_mine and arr_check_open[index - width_sq - 1] == close_cell) { //если не бомба и не открыта
			if (arr_check[index - width_sq - 1] >= one and arr_check[index - width_sq - 1] <= eight) //если число 
				arr_check_open[index - width_sq - 1] = open_cell;
			else {
				if (arr_check[index - width_sq - 1] == empty_sq) //если пустая клетка
					arr_check_open[index - width_sq - 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index - width_sq - 1);
			}
		}
		if (arr_check[index - width_sq + 1] != sp_mine and index - width_sq + 1 < width_sq * height_sq and arr_check_open[index - width_sq + 1] == close_cell and (index - width_sq + 1) % width_sq != 0) { //если не бомба и не открыта
			if (arr_check[index - width_sq + 1] >= one and arr_check[index - width_sq + 1] <= eight) //если число 
				arr_check_open[index - width_sq + 1] = open_cell;
			else {
				if (arr_check[index - width_sq + 1] == empty_sq) //если пустая клетка
					arr_check_open[index - width_sq + 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index - width_sq + 1);
			}
		}
	}
	if (index + width_sq < width_sq * height_sq and arr_check[index] == empty_sq) { //нижняя полоса
		if (arr_check[index + width_sq] != sp_mine and arr_check_open[index + width_sq] == close_cell) { //если не бомба и не открыта
			if (arr_check[index + width_sq] >= one and arr_check[index + width_sq] <= eight) //если число 
				arr_check_open[index + width_sq] = open_cell;
			else {
				if (arr_check[index + width_sq] == empty_sq) //если пустая клетка
					arr_check_open[index + width_sq] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index + width_sq);
			}
		}
		if (arr_check[index + width_sq - 1] != sp_mine and index + width_sq - 1 > 0 and (index + width_sq - 1) % width_sq != width_sq - 1 and arr_check_open[index + width_sq - 1] == close_cell) { //если не бомба и не открыта
			if (arr_check[index + width_sq - 1] >= one and arr_check[index + width_sq - 1] <= eight) //если число 
				arr_check_open[index + width_sq - 1] = open_cell;
			else {
				if (arr_check[index + width_sq - 1] == empty_sq) //если пустая клетка
					arr_check_open[index + width_sq - 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index + width_sq - 1);
			}
		}
		if (arr_check[index + width_sq + 1] != sp_mine and index + width_sq + 1 < width_sq * height_sq and (index + width_sq + 1) % width_sq != 0 and arr_check_open[index + width_sq + 1] == close_cell) { //если не бомба и не октрыта
			if (arr_check[index + width_sq + 1] >= one and arr_check[index + width_sq + 1] <= eight) //если число 
				arr_check_open[index + width_sq + 1] = open_cell;
			else {
				if (arr_check[index + width_sq + 1] == empty_sq) //если пустая клетка
					arr_check_open[index + width_sq + 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index + width_sq + 1);
			}
		}
	}
	if (index - 1 > 0 and (index - 1) % width_sq != width_sq - 1 and arr_check[index] == empty_sq) { //левая полоса
		if (arr_check[index - 1] != sp_mine and arr_check_open[index - 1] == close_cell) //если не бомба и не открыта
		{
			if (arr_check[index - 1] >= one and arr_check[index - 1] <= eight) //если число 
				arr_check_open[index - 1] = open_cell;
			else {
				if (arr_check[index - 1] == empty_sq) //если пустая клетка
					arr_check_open[index - 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index - 1);
			}
		}
	}
	if (index + 1 < width_sq * height_sq and (index + 1) % width_sq != 0 and arr_check[index] == empty_sq) { //правая полоса
		if (arr_check[index + 1] != sp_mine and arr_check_open[index + 1] == close_cell) //если не бомба и не открыта
		{
			if (arr_check[index + 1] >= one and arr_check[index + 1] <= eight) //если число 
				arr_check_open[index + 1] = open_cell;
			else {
				if (arr_check[index + 1] == empty_sq) //если пустая клетка
					arr_check_open[index + 1] = open_cell; //открываем клетку
				empty_cell(width_sq, height_sq, index + 1);
			}
		}	
	}
}

void minesweeper(int width_sq, int height_sq, int num_mine) {
	int height; int width; //размеры игрового поля в пикселях
	int win_lose = 0; //игра = 0, победа = 1, проигрыш = -1
	int cnt_flag = 0; //подсчёт кол-ва правильно поставленных флагов

	//подсчёт размера игрового поля
	width = width_sq * texture_side;
	height = height_sq * texture_side;

	//создание текстуры и спрайта
	Texture texture;
	texture.loadFromFile("C:\\Users\\ASUS\\Desktop\\Занятия\\Программирование\\2 сем\\Курсовая\\рисунок.jpg"); //выгрузка текстуры
	Sprite sprite[num_sq]; //массив спрайтов
	for (int i = 0; i < num_sq; i++) { //цикл создания массива спрайтов
		sprite[i].setTexture(texture);
		sprite[i].setTextureRect(IntRect(i * texture_side, 0, texture_side, texture_side)); //вырезаем нужный квадратик из текстуры
	}

	//спрайты проигрыша и победы
	Texture texture1;
	texture1.loadFromFile("C:\\Users\\ASUS\\Desktop\\Занятия\\Программирование\\2 сем\\Курсовая\\Im.jpg"); //выгрузка текстуры
	Sprite sprite_win_lose[2]; //массив спрайтов
	sprite_win_lose[0].setTexture(texture1); //Проигрыш
	sprite_win_lose[0].setTextureRect(IntRect(0, 0, width_win_lose, height_win_lose));
	sprite_win_lose[1].setTexture(texture1); //Выигрыш
	sprite_win_lose[1].setTextureRect(IntRect(0, height_win_lose, width_win_lose, height_win_lose));

	RenderWindow window(VideoMode(width + dop_width_win, height), L"Minesweeper"); // создаем окно 
	while (window.isOpen()) //пока открыто окно
	{
		Event event;
		while (window.pollEvent(event)) {//проверяем события окна
			if (event.type == Event::Closed) window.close(); //закрываем окно, если был "запрос закрытия"

			window.clear(Color::White); //задаём цвет фона

			int index; //индекс массива arr_check;
			

			for (int j = 0; j < height_sq; j++) { //цикл загрузки в окне сетки спрайтов
				for (int i = 0; i < width_sq; i++) {
					if (arr_check_open[i * width_sq + j] == open_cell) {
						sprite[arr_check[i * width_sq + j]].setPosition(j * texture_side, i * texture_side); //устанавливаем спрайт в нужную позицию
						window.draw(sprite[arr_check[i * width_sq + j]]); //рисуем спрайт
						if (win_lose == lose) { //если проигрыш
							sprite_win_lose[0].setPosition(width + pos_x_win_lose, pos_y_win_lose);
							window.draw(sprite_win_lose[0]);
						}
						else if (win_lose == win) { //если победа
							sprite_win_lose[1].setPosition(width + pos_x_win_lose, pos_y_win_lose);
							window.draw(sprite_win_lose[1]);
						}
					}
					else if(arr_check_open[i * width_sq + j] == flag_cell){ //стоит флаг
						sprite[flag].setPosition(j * texture_side, i * texture_side); //устанавливаем спрайт флага в нужную позицию
						window.draw(sprite[flag]); //рисуем спрайт
					}
					else {
						sprite[sq].setPosition(j * texture_side, i * texture_side); //устанавливаем спрайт в нужную позицию
						window.draw(sprite[sq]); //рисуем спрайт
					}
				}
			}

			if (event.type == Event::MouseButtonPressed) { //нажатие кнопки 
				if (event.mouseButton.button == Mouse::Left and event.mouseButton.x < width) { //если нажата левая кнопка мышки, то просто открывается клетка
					index = (event.mouseButton.x / texture_side) + (event.mouseButton.y / texture_side) * width_sq;
					if (arr_check_open[index] == close_cell) { //проверка на то, была ли открыта клетка 

						if (arr_check[index] == sp_mine) { //если выбранная клетка - бомба, т.е. проигрыш
							win_lose = lose;
							for (int i = 0; i < width_sq; i++) { //открываем все ячейки
								for (int j = 0; j < height_sq; j++) {
									arr_check_open[i * width_sq + j] = open_cell; //открываем всё поле
								}
							}
						}
						else {
							if (arr_check[index] == empty_sq) {//если выбранная клетка - пустая
								empty_cell(width_sq, height_sq, index);
							}
							else { //если выбранная клетка - цифра
								arr_check_open[index] = open_cell;
							}
						}
					}
				}
				if (event.mouseButton.button == Mouse::Right and event.mouseButton.x < width) { //если нажата правая кнопка мышки, то ставится флажок
					index = (event.mouseButton.x / texture_side) + (event.mouseButton.y / texture_side) * width_sq;
					if (arr_check_open[index] == close_cell) { //проверка на то, закрыта ли эта клетка
						arr_check_open[index] = flag_cell;//соответсвует добавления флага
						if (arr_check[index] == sp_mine) {
							cnt_flag+=1;
							if (cnt_flag == num_mine) { //победа
								win_lose = win;
								for (int i = 0; i < width_sq; i++) { //открываем все ячейки
									for (int j = 0; j < height_sq; j++) {
										arr_check_open[i * width_sq + j] = open_cell; //открываем всё поле
									}
								}
							}
						}
					}
				}
			}
			window.display(); //всё выводим в буфер
		}
	}
}

int main()
{
	SetConsoleCP(RUS);			    // разрешить русский текст в потоке ввода
	SetConsoleOutputCP(RUS);		// разрешить русский текст в потоке вывода
	srand(time(NULL)); //для генерации случайных чисел 

	int selecting_mode; // переменная выбора режима игры
	int width_sq; int height_sq; // ширина и высота игрового поля 
	int num_mine; //кол-во мин на игровом поле
	bool error_selecting_mode; //проверка на то, правильно ли был выбран режим

	cout << "Выберите режим игры:\n<1> Новичок\n<2> Любитель\n<3> Профессионал" << endl;
	cin >> selecting_mode;
	//выбор режима игры
	switch (selecting_mode) {
	case 1:
		width_sq = width_beginner; height_sq = height_beginner; //задаём размеры игрового поля в квадратиках (по 32x32)
		num_mine = mine_beginner; //задаём кол-во мин
		error_selecting_mode = false;
		break;
	case 2:
		width_sq = width_amateur; height_sq = height_amateur; //задаём размеры игрового поля в квадратиках (по 32x32)
		num_mine = mine_amateur; //задаём кол-во мин
		error_selecting_mode = false;
		break;
	case 3:
		width_sq = width_professional; height_sq = height_professional; //задаём размеры игрового поля в квадратиках (по 32x32)
		num_mine = mine_professional; //задаём кол-во мин
		error_selecting_mode = false;
		break;
	default:
		error_selecting_mode = true;
		break;
	}

	if (error_selecting_mode == false) {
		mine(width_sq, height_sq, num_mine);
		minesweeper(width_sq, height_sq, num_mine);
	}
	return 0;
}