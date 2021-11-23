#include <SFML/Graphics.hpp>
#include <iostream> 
#include <windows.h>   // для SetConsoleCP()
#include <stdio.h> //для sleep(300) - задержка в 0,3 секунды
#include <fstream> //для работы с файлами
#include <cmath>

using namespace sf;
using namespace std;

#define RUS 1251
#define ind_x 0
#define ind_y 1
#define zero 0

bool first_stetch = true; //проверка на первый стежок
int num_coord = 0; //кол-во координат
int max_len_stetch; //максимальная длина стежка
int width = 800; //размер окна
int height = 600;
int moveX, moveY, moveStep = 10; //переменные движения внутри окна
float zoom = 1; //во сколько раз увеличивается или уменьшается масштаб
float zoom_in_step = 1, zoom_out_step = 0.25; //переменные изменения масштаба в сторону увеличения и уменьшения соответсвенно
float zoom_in_max = 3, zoom_out_max = 0.5; //максимальное увеличение и уменьшение масштаба
int num_grid = 100; //кол-во линий сетки 
int sq_grid = 40; //сторона квадратика сетки


struct coordinates { //динамический список из координат точек, показывающих смещение относительно предыдущей точки
	int x;
	int y;
	struct coordinates* next; //переход к следующему элементу списка
	struct coordinates* tail; //последний элемент 
};
coordinates* head = NULL;
coordinates* current; //указатель на текущий элемент
coordinates* tail = nullptr; //указатель на последний элемент

void load_game(){ //функция подгрузки сохранённого рисунка
	ifstream fin("coordinates.txt"); //открываем файл для считывания
	if (!fin.is_open()) { //проверка на успешность открытия файла
		cout << "\nОшибка открытия файла" << endl;
	}

	//считываем первую координату из файла в head
	head = new coordinates; 
	fin >> head->x >> head->y;
	head->next = NULL;
	tail = head;
	current = new coordinates;

	num_coord++; //увеличиваем кол-во координат
	first_stetch = 0;
	
	//считываем остальные координаты
	while (!fin.eof()) { //пока не конец файла
		current = new coordinates;
		fin >> current->x >> current->y;
		if (fin.eof()) //преждевременный выход
			break;
		tail->next = current; 
		current->next = NULL; 
		tail = current;
		num_coord++; //увеличиваем кол-во координат
	}

	fin.close(); //закрываем файл
}

void save_game() { //функция сохранения рисунка
	ofstream file_input("coordinates.txt", ofstream::trunc); // открываем файл для записи
	if (!file_input.is_open()) { //проверка на успешность открытия файла
		cout << "\nОшибка открытия файла" << endl;
	}
	current = head;
	while (current != NULL) { //записываем все координаты в файл построчно
		file_input << current->x << '\n' << current->y << '\n';
		current = current->next;
	}
	
	file_input.close(); //закрываем файл
}

void delete_struct() { //функция удаления списка и обнуления всех значений, заданных по ходу программы
	current = head;
	coordinates* del;
	while (current != NULL) {
		del = current->next;
		delete current;
		current = del;
	}
	head = NULL;
	first_stetch = 1;
	tail = nullptr;
	zoom = 1;
	moveX = 0;
	moveY = 0;
	num_coord = 0;
}

void drawing_grid(sf::RenderWindow& window) { //функция рисования сетки
	sf::VertexArray grid_vert(sf::Lines, 2);	//создаем массив точек для задания верикальных линий сетки
	sf::VertexArray grid_horiz(sf::Lines, 2);	//создаем массив точек для задания горизонатльных линий сетки
	for (int i = 0; i < num_grid; i++) {
		//линии рисуются относительно начальной координаты в разные стороны
		grid_vert[0].position = Vector2f(head->x + i * sq_grid * zoom + moveX, 0);
		grid_vert[1].position = Vector2f(head->x + i * sq_grid * zoom + moveX, width);
		grid_vert[0].color = sf::Color::Green;
		grid_vert[1].color = sf::Color::Green;
		grid_horiz[0].position = Vector2f(0, head->y + i * sq_grid * zoom + moveY);
		grid_horiz[1].position = Vector2f(width, head->y + i * sq_grid * zoom + moveY);
		grid_horiz[0].color = sf::Color::Green;
		grid_horiz[1].color = sf::Color::Green;
		window.draw(grid_horiz);
		window.draw(grid_vert);
		grid_vert[0].position = Vector2f(head->x - i * sq_grid * zoom + moveX, 0);
		grid_vert[1].position = Vector2f(head->x - i * sq_grid * zoom + moveX, width);
		grid_vert[0].color = sf::Color::Green;
		grid_vert[1].color = sf::Color::Green;
		grid_horiz[0].position = Vector2f(0, head->y - i * sq_grid * zoom + moveY);
		grid_horiz[1].position = Vector2f(width, head->y - i * sq_grid * zoom + moveY);
		grid_horiz[0].color = sf::Color::Green;
		grid_horiz[1].color = sf::Color::Green;
		window.draw(grid_horiz);
		window.draw(grid_vert);
	}

}

void _stitches(sf::RenderWindow& window, int x, int y) { //заполнение списка координат
	//первая точка задаётся в действительных координатах, остальные в относительных, то есть показывают перемещение относительно первой точки
	if (first_stetch == true) { //поставлена только первая точка
		num_coord++;

		if (head == NULL) { //создание начала списка и заполнение координатами (0;0)
			head = new coordinates;
			head->x = x;
			head->y = y;
			head->next = NULL;
			tail = head;
		}
		current = head;

		first_stetch = false;
	}
	else {
		int xNew = (x - head->x) / zoom, yNew = (head->y - y) / zoom; //пересчитываем координаты из относительных в действительные и без масштабирования

		if (floor(sqrt((pow(xNew - (head == tail ? 0 : tail->x), 2) + pow(yNew - (head == tail ? 0 : tail->y), 2)))) <= max_len_stetch) {  //проверяем, чтобы длина стежка не была больше заданной
			//первый элемент считается списка считается за ноль, т.е. длина всегда будет меньше
			num_coord++;

			//заполняем список координатами СМЕЩЕНИЯ без перехода к новому элементу
			current = new coordinates;
			current->x = xNew;
			current->y = yNew;
			tail->next = current; //Поле next предыдущего элемента указывает на текущий элемент
			current->next = NULL;
			tail = current; //Текущий элемент - последний в списке
		}
	}
}

void drawing_circle(sf::RenderWindow& window) { //рисование круга для обозначения максимальной длины стежка
	sf::CircleShape Circle(max_len_stetch * zoom); //задаём радиус

	//позиция круга задаётся с учётом того, что центр должен находиться на конце последнего стежка, если у нас поставлена только одна точка,
	//то мы не переводим координаты в действиетльные, если не первая, то переводим в действительные
	Circle.setPosition((tail == head ? 0 : tail->x * zoom) + head->x - max_len_stetch * zoom + moveX, (tail == head ? 0 : -tail->y * zoom) + head->y - max_len_stetch * zoom + moveY);
	Circle.setOutlineThickness(2.f); //толщина круга
	Circle.setFillColor(sf::Color(255, 255, 255, 0)); //задаём цвет круга прозрачный
	Circle.setOutlineColor(sf::Color::Red); //цвет контура
	window.draw(Circle); //вывод круга
}

void drawing_stiches(sf::RenderWindow& window) {//рисование стежков
	//moveX и moveY показывают движение рисунка при перемещении в окне

	sf::VertexArray line(sf::Lines, 2);	//создаем массив точек для задания линий
	coordinates* buff_coord, * buff_coord2; //два последующих элемента списка
	buff_coord = head;
	buff_coord2 = head->next;
	//рисуем первую линию, где первая точка в действительных координатах с учётом движения и масштабирования
	//первая точка первой линии не масштабируется, т.к. относительно неё идёт масштабирование
	line[0].position = sf::Vector2f(buff_coord->x + moveX, buff_coord->y + moveY); //позиция начала линии 
	line[0].color = sf::Color::Blue; //задаём цвет начала линии
	line[1].position = sf::Vector2f(buff_coord2->x * zoom + head->x + moveX, -buff_coord2->y * zoom + head->y + moveY); //позиция конца линии с учётом масштаба и движения
	line[1].color = sf::Color::Blue;
	window.draw(line);
	buff_coord = buff_coord2;
	buff_coord2 = buff_coord2->next; //переходим к следующему элементу списка

	//рисуем оставшиеся линии, где точки в относительных координатах
	for (int i = 0; i < num_coord - 2; i++) {
		line[0].position = sf::Vector2f(buff_coord->x * zoom + head->x + moveX, -buff_coord->y * zoom + head->y + moveY); //позиция начала линии
		line[0].color = sf::Color::Blue; //задаём цвет начала линии
		line[1].position = sf::Vector2f(buff_coord2->x * zoom + head->x + moveX, -buff_coord2->y * zoom + head->y + moveY); //позиция конца линии
		line[1].color = sf::Color::Blue; //задаём цвет линии
		buff_coord = buff_coord2;
		buff_coord2 = buff_coord2->next;
		window.draw(line); //рисуем линию
	}
}

void win() {//основная функция работы с окном
	cout << "Введите максимальную длину стежка" << endl;
	cin >> max_len_stetch;

	int x, y; //текущие координаты

	sf::RenderWindow window(sf::VideoMode(width, height), L"Sewing machine"); // создаем окно 

	while (window.isOpen()) //пока открыто окно
	{
		window.clear(); //очищаем окно
		sf::Event event;

		while (window.pollEvent(event)) {//проверяем события окна
			if (event.type == sf::Event::Closed) window.close(); //закрываем окно, если был "запрос закрытия"

			//масштабирование
			if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
				if (event.mouseWheelScroll.delta > zero) { // увеличение масштаба
					zoom += zoom_in_step;
					zoom = (zoom >= zoom_in_max ? zoom_in_max : zoom); //зум не более 3 
				}
				if ((event.mouseWheelScroll.delta < zero)) { // уменьшение масштаба
					zoom -= zoom_out_step;
					zoom = (zoom <= zoom_out_max ? zoom_out_max : zoom); //зум не менее, чем в 0,5
				}
			}

			//движение внутри окна
			if ((event.type == sf::Event::KeyPressed)) {
				if (event.key.code == sf::Keyboard::Up) { //движение вверх
					moveY -= moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Down) { //движение вниз
					moveY += moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Right) { //движение вправо
					moveX += moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Left) { //движение влево
					moveX -= moveStep;
					Sleep(300);
				}
			}

			//удаление последнего стежка
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace && num_coord > 1) { //кол-во стежков должно быть 2 и больше
				coordinates* del = head; //указатель на первый элемент списка
				while (del->next != tail) {//проходим по всем элементам (останавливаемся на предпоследнем элементе)
					del = del->next; //переход к следующему элементу списка
				}
				tail = del; //последний элемент становится предпоследним
				tail->next = NULL; //очищение последнего элемента
				delete del->next; //удаление последнего элемента
				num_coord--; //уменьшение кол-ва элементов списка
			}

			if (event.type == sf::Event::MouseButtonPressed) { //нажатие кнопки 
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					x = event.mouseButton.x;
					y = event.mouseButton.y;
					_stitches(window, event.mouseButton.x - moveX, event.mouseButton.y - moveY); //задание координат без движения и масштабирования
				}
			}
		}

		if (num_coord >= 2) //рисование линии, если было поставлено хотя бы две точки
			drawing_stiches(window);
		if (num_coord > 0) {//рисование обозначительного круга и сетки, если была поставлена хотя бы одна точка
			drawing_grid(window);
			drawing_circle(window);
		}
		window.display(); //буфер отображается на экране
	}
}

int main()
{
	SetConsoleCP(RUS);			    // разрешить русский текст в потоке ввода
	SetConsoleOutputCP(RUS);		// разрешить русский текст в потоке вывода

	int choice;
	int choice_save; //сохранять или нет

	do {
		cout << "Выберите действие\n<1>: Начать новый рисунок\n<2>: Продолжить рисунок\n<3>: Посмотреть инструкцию\n<4>: Завершить работу\n";
		cin >> choice;
		if (choice <= 0 or choice >= 5) cout << "Неверный ввод" << endl;
		else {
			switch (choice) { //верхнее меню
			case 1:
				delete_struct(); //очистка списка
				win();
				cout << "Если хотите сохранить рисунок, нажмите <1>, если нет, нажмите любую цифру" << endl;
				cin >> choice_save;
				if (choice_save == 1) {
					save_game(); //сохранение рисунка
				}
				break;
			case 2:
				delete_struct(); //очитска списка
				load_game(); //подгрузка рисунка
				win();
				cout << "Если хотите сохранить рисунок, нажмите <1>, если нет, нажмите любую цифру" << endl;
				cin >> choice_save;
				if (choice_save == 1) {
					save_game(); //сохранение игры
				}
				break;
			case 3:
				cout << "Ставьте стежки внутри круга, так длина стежка не будет превышать максимальной\nНажмите <Backspace>, чтобы удалить последний стежок\n";
				cout <<"Прокурутите мышку, чтобы увеличить или уменьшить масштаб\nНажмите на клавиши <Вверх>, <Вниз> и т.д., чтобы переместить рисунок внутри окна";
				break;
			}

		}
	} while (choice != 4);
	
	return 0;
}
