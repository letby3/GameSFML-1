#include <SFML/Graphics.hpp>
#include <iostream> 
#include <windows.h>   // ��� SetConsoleCP()
#include <stdio.h> //��� sleep(300) - �������� � 0,3 �������
#include <fstream> //��� ������ � �������
#include <cmath>

using namespace sf;
using namespace std;

#define RUS 1251
#define ind_x 0
#define ind_y 1
#define zero 0

bool first_stetch = true; //�������� �� ������ ������
int num_coord = 0; //���-�� ���������
int max_len_stetch; //������������ ����� ������
int width = 800; //������ ����
int height = 600;
int moveX, moveY, moveStep = 10; //���������� �������� ������ ����
float zoom = 1; //�� ������� ��� ������������� ��� ����������� �������
float zoom_in_step = 1, zoom_out_step = 0.25; //���������� ��������� �������� � ������� ���������� � ���������� �������������
float zoom_in_max = 3, zoom_out_max = 0.5; //������������ ���������� � ���������� ��������
int num_grid = 100; //���-�� ����� ����� 
int sq_grid = 40; //������� ���������� �����


struct coordinates { //������������ ������ �� ��������� �����, ������������ �������� ������������ ���������� �����
	int x;
	int y;
	struct coordinates* next; //������� � ���������� �������� ������
	struct coordinates* tail; //��������� ������� 
};
coordinates* head = NULL;
coordinates* current; //��������� �� ������� �������
coordinates* tail = nullptr; //��������� �� ��������� �������

void load_game(){ //������� ��������� ����������� �������
	ifstream fin("coordinates.txt"); //��������� ���� ��� ����������
	if (!fin.is_open()) { //�������� �� ���������� �������� �����
		cout << "\n������ �������� �����" << endl;
	}

	//��������� ������ ���������� �� ����� � head
	head = new coordinates; 
	fin >> head->x >> head->y;
	head->next = NULL;
	tail = head;
	current = new coordinates;

	num_coord++; //����������� ���-�� ���������
	first_stetch = 0;
	
	//��������� ��������� ����������
	while (!fin.eof()) { //���� �� ����� �����
		current = new coordinates;
		fin >> current->x >> current->y;
		if (fin.eof()) //��������������� �����
			break;
		tail->next = current; 
		current->next = NULL; 
		tail = current;
		num_coord++; //����������� ���-�� ���������
	}

	fin.close(); //��������� ����
}

void save_game() { //������� ���������� �������
	ofstream file_input("coordinates.txt", ofstream::trunc); // ��������� ���� ��� ������
	if (!file_input.is_open()) { //�������� �� ���������� �������� �����
		cout << "\n������ �������� �����" << endl;
	}
	current = head;
	while (current != NULL) { //���������� ��� ���������� � ���� ���������
		file_input << current->x << '\n' << current->y << '\n';
		current = current->next;
	}
	
	file_input.close(); //��������� ����
}

void delete_struct() { //������� �������� ������ � ��������� ���� ��������, �������� �� ���� ���������
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

void drawing_grid(sf::RenderWindow& window) { //������� ��������� �����
	sf::VertexArray grid_vert(sf::Lines, 2);	//������� ������ ����� ��� ������� ����������� ����� �����
	sf::VertexArray grid_horiz(sf::Lines, 2);	//������� ������ ����� ��� ������� �������������� ����� �����
	for (int i = 0; i < num_grid; i++) {
		//����� �������� ������������ ��������� ���������� � ������ �������
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

void _stitches(sf::RenderWindow& window, int x, int y) { //���������� ������ ���������
	//������ ����� ������� � �������������� �����������, ��������� � �������������, �� ���� ���������� ����������� ������������ ������ �����
	if (first_stetch == true) { //���������� ������ ������ �����
		num_coord++;

		if (head == NULL) { //�������� ������ ������ � ���������� ������������ (0;0)
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
		int xNew = (x - head->x) / zoom, yNew = (head->y - y) / zoom; //������������� ���������� �� ������������� � �������������� � ��� ���������������

		if (floor(sqrt((pow(xNew - (head == tail ? 0 : tail->x), 2) + pow(yNew - (head == tail ? 0 : tail->y), 2)))) <= max_len_stetch) {  //���������, ����� ����� ������ �� ���� ������ ��������
			//������ ������� ��������� ������ ��������� �� ����, �.�. ����� ������ ����� ������
			num_coord++;

			//��������� ������ ������������ �������� ��� �������� � ������ ��������
			current = new coordinates;
			current->x = xNew;
			current->y = yNew;
			tail->next = current; //���� next ����������� �������� ��������� �� ������� �������
			current->next = NULL;
			tail = current; //������� ������� - ��������� � ������
		}
	}
}

void drawing_circle(sf::RenderWindow& window) { //��������� ����� ��� ����������� ������������ ����� ������
	sf::CircleShape Circle(max_len_stetch * zoom); //����� ������

	//������� ����� ������� � ������ ����, ��� ����� ������ ���������� �� ����� ���������� ������, ���� � ��� ���������� ������ ���� �����,
	//�� �� �� ��������� ���������� � ��������������, ���� �� ������, �� ��������� � ��������������
	Circle.setPosition((tail == head ? 0 : tail->x * zoom) + head->x - max_len_stetch * zoom + moveX, (tail == head ? 0 : -tail->y * zoom) + head->y - max_len_stetch * zoom + moveY);
	Circle.setOutlineThickness(2.f); //������� �����
	Circle.setFillColor(sf::Color(255, 255, 255, 0)); //����� ���� ����� ����������
	Circle.setOutlineColor(sf::Color::Red); //���� �������
	window.draw(Circle); //����� �����
}

void drawing_stiches(sf::RenderWindow& window) {//��������� �������
	//moveX � moveY ���������� �������� ������� ��� ����������� � ����

	sf::VertexArray line(sf::Lines, 2);	//������� ������ ����� ��� ������� �����
	coordinates* buff_coord, * buff_coord2; //��� ����������� �������� ������
	buff_coord = head;
	buff_coord2 = head->next;
	//������ ������ �����, ��� ������ ����� � �������������� ����������� � ������ �������� � ���������������
	//������ ����� ������ ����� �� ��������������, �.�. ������������ �� ��� ���������������
	line[0].position = sf::Vector2f(buff_coord->x + moveX, buff_coord->y + moveY); //������� ������ ����� 
	line[0].color = sf::Color::Blue; //����� ���� ������ �����
	line[1].position = sf::Vector2f(buff_coord2->x * zoom + head->x + moveX, -buff_coord2->y * zoom + head->y + moveY); //������� ����� ����� � ������ �������� � ��������
	line[1].color = sf::Color::Blue;
	window.draw(line);
	buff_coord = buff_coord2;
	buff_coord2 = buff_coord2->next; //��������� � ���������� �������� ������

	//������ ���������� �����, ��� ����� � ������������� �����������
	for (int i = 0; i < num_coord - 2; i++) {
		line[0].position = sf::Vector2f(buff_coord->x * zoom + head->x + moveX, -buff_coord->y * zoom + head->y + moveY); //������� ������ �����
		line[0].color = sf::Color::Blue; //����� ���� ������ �����
		line[1].position = sf::Vector2f(buff_coord2->x * zoom + head->x + moveX, -buff_coord2->y * zoom + head->y + moveY); //������� ����� �����
		line[1].color = sf::Color::Blue; //����� ���� �����
		buff_coord = buff_coord2;
		buff_coord2 = buff_coord2->next;
		window.draw(line); //������ �����
	}
}

void win() {//�������� ������� ������ � �����
	cout << "������� ������������ ����� ������" << endl;
	cin >> max_len_stetch;

	int x, y; //������� ����������

	sf::RenderWindow window(sf::VideoMode(width, height), L"Sewing machine"); // ������� ���� 

	while (window.isOpen()) //���� ������� ����
	{
		window.clear(); //������� ����
		sf::Event event;

		while (window.pollEvent(event)) {//��������� ������� ����
			if (event.type == sf::Event::Closed) window.close(); //��������� ����, ���� ��� "������ ��������"

			//���������������
			if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
				if (event.mouseWheelScroll.delta > zero) { // ���������� ��������
					zoom += zoom_in_step;
					zoom = (zoom >= zoom_in_max ? zoom_in_max : zoom); //��� �� ����� 3 
				}
				if ((event.mouseWheelScroll.delta < zero)) { // ���������� ��������
					zoom -= zoom_out_step;
					zoom = (zoom <= zoom_out_max ? zoom_out_max : zoom); //��� �� �����, ��� � 0,5
				}
			}

			//�������� ������ ����
			if ((event.type == sf::Event::KeyPressed)) {
				if (event.key.code == sf::Keyboard::Up) { //�������� �����
					moveY -= moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Down) { //�������� ����
					moveY += moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Right) { //�������� ������
					moveX += moveStep;
					Sleep(300);
				}
				if (event.key.code == sf::Keyboard::Left) { //�������� �����
					moveX -= moveStep;
					Sleep(300);
				}
			}

			//�������� ���������� ������
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace && num_coord > 1) { //���-�� ������� ������ ���� 2 � ������
				coordinates* del = head; //��������� �� ������ ������� ������
				while (del->next != tail) {//�������� �� ���� ��������� (��������������� �� ������������� ��������)
					del = del->next; //������� � ���������� �������� ������
				}
				tail = del; //��������� ������� ���������� �������������
				tail->next = NULL; //�������� ���������� ��������
				delete del->next; //�������� ���������� ��������
				num_coord--; //���������� ���-�� ��������� ������
			}

			if (event.type == sf::Event::MouseButtonPressed) { //������� ������ 
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					x = event.mouseButton.x;
					y = event.mouseButton.y;
					_stitches(window, event.mouseButton.x - moveX, event.mouseButton.y - moveY); //������� ��������� ��� �������� � ���������������
				}
			}
		}

		if (num_coord >= 2) //��������� �����, ���� ���� ���������� ���� �� ��� �����
			drawing_stiches(window);
		if (num_coord > 0) {//��������� ���������������� ����� � �����, ���� ���� ���������� ���� �� ���� �����
			drawing_grid(window);
			drawing_circle(window);
		}
		window.display(); //����� ������������ �� ������
	}
}

int main()
{
	SetConsoleCP(RUS);			    // ��������� ������� ����� � ������ �����
	SetConsoleOutputCP(RUS);		// ��������� ������� ����� � ������ ������

	int choice;
	int choice_save; //��������� ��� ���

	do {
		cout << "�������� ��������\n<1>: ������ ����� �������\n<2>: ���������� �������\n<3>: ���������� ����������\n<4>: ��������� ������\n";
		cin >> choice;
		if (choice <= 0 or choice >= 5) cout << "�������� ����" << endl;
		else {
			switch (choice) { //������� ����
			case 1:
				delete_struct(); //������� ������
				win();
				cout << "���� ������ ��������� �������, ������� <1>, ���� ���, ������� ����� �����" << endl;
				cin >> choice_save;
				if (choice_save == 1) {
					save_game(); //���������� �������
				}
				break;
			case 2:
				delete_struct(); //������� ������
				load_game(); //��������� �������
				win();
				cout << "���� ������ ��������� �������, ������� <1>, ���� ���, ������� ����� �����" << endl;
				cin >> choice_save;
				if (choice_save == 1) {
					save_game(); //���������� ����
				}
				break;
			case 3:
				cout << "������� ������ ������ �����, ��� ����� ������ �� ����� ��������� ������������\n������� <Backspace>, ����� ������� ��������� ������\n";
				cout <<"����������� �����, ����� ��������� ��� ��������� �������\n������� �� ������� <�����>, <����> � �.�., ����� ����������� ������� ������ ����";
				break;
			}

		}
	} while (choice != 4);
	
	return 0;
}
