#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

struct Emu_Command
{
	char prefix;
	int floor;
};

Emu_Command parse_command();

class Elevator {
public:
	Elevator(unsigned int e_floors) :floors(e_floors) {
		current_floor = 1;
		for (int i = 0; i < floors; i++)
		{
			orders[i] = false;
		}
	}
	~Elevator() {
		delete[] orders;
	}

	void print_elevator_status() {
		cout << "Cabin on floor " << current_floor << ".";
		switch (direction)
		{
		case 1:
			cout << " Going upwards. ";
			break;
		case 0:
			cout << " Idle. ";
			break;
		case -1:
			cout << " Going downwards. ";
			break;
		default:
			break;
		}
		cout << endl;
	}

	void run_command(Emu_Command command) {
		if (command.prefix == 'C')
		{
			orders[command.floor - 1] = true;
		}
		if (command.prefix == 'F')
		{
			orders[command.floor - 1] = true;
		}
		check_floors();
	}

private:
	unsigned int const floors;			// Предположим, что задение не имеет подземных этажей и объявим 
	unsigned int current_floor;			// беззнаковые переменные для хранения этажности дома, текущего этажа.
	int direction = 0;
	bool *orders{ new bool[floors] };

	void check_floors() {
		int orders_upper = 0;
		int orders_lower = 0;
		for (int i = current_floor; i < floors; i++)	//Проверяем вызовы на этажи выше
		{
			if (orders[i]) orders_upper++;
		}
		for (int i = 0; i < current_floor - 1; i++)				//Проверяем вызовы на этажи ниже
		{
			if (orders[i]) orders_lower++;
		}
		if (orders[current_floor - 1])			//Если есть вызов на текущем этаже, делаем остановку
		{
			make_stop();
		}
		
		if (!orders_upper && !orders_lower)					//Если нет вызовов, останавливаем лифт
		{
			direction = 0;
			print_elevator_status();
		}

		if (direction == 0)									//Если лифт в режиме ожидания, выбирается направление движения в случае появления вызова
		{
			if (orders_lower - orders_upper > 0) direction = -1;
			if (orders_lower - orders_upper < 0) direction = 1;
		}

		//Смена направления движения:
		if (orders_upper == 0 && direction == 1)
		{
			if (orders_lower != 0) direction = -1;
			else direction = 0;			
		}
		if (orders_lower == 0 && direction == -1) 
		{
			if (orders_upper != 0) direction = 1;
			else direction = 0;
		}

		if (direction != 0) make_move();
	}

	void make_move() {
		//delay 1 sec
		this_thread::sleep_for(chrono::milliseconds(1000));
		current_floor = current_floor + direction;
		print_elevator_status();
		check_floors();
	}

	void make_stop() {
		cout << endl << "Stop on floor " << current_floor << " Doors Opend." << endl;
		orders[current_floor - 1] = false;
		//delay 1 sec
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "Doors closed." << endl;
	}
};

int main() {

	setlocale(0, "Russian");
	int input_floors;
	cout << "Available elevator commands:" << endl
		<< "F<floor> - calling the elevator from the floor specified;" << endl
		<< "C<floor> - an order from the cabin to go to the floor specified;" << endl;
		//<< "P - Pouse \ Continue elevator muvement;" << endl
		//<< "S - Termenate the emulation." << endl << endl;

	cout << "Enter the number of floors (from 2 to 999): ";
	do{													
		cin >> input_floors;			//Ввод количства этажей
		if (cin.fail() || input_floors <= 1 || input_floors >= 1000)	//Обработка ошибки ввода и повторный запрос на введение числа этажей
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Wrong input. Please, enter the number of floors: ";
		}
	} while (input_floors <= 1 || input_floors >= 1000);
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	Elevator elevator(input_floors);
	elevator.print_elevator_status();

	Emu_Command emu_command;

	do
	{
		do
		{
			emu_command = parse_command();			//Ввод команды для лифта
			if (emu_command.floor > input_floors)	//Обработка ошибки ввода этажа в команде
			{
				cout << "No such floor. Repeat the input: ";
			}
		} while (emu_command.floor > input_floors);
		elevator.run_command(emu_command);
		system("pause");
	} while (true);

	return 0;
}

Emu_Command parse_command() {
	char command_buffer[100];		//Буфер для команды управления лифтом
	cout << "Command: ";
	bool input_err = false;
	Emu_Command emu_command;
	do
	{
		input_err = false;				
		if (cin.fail())								//Обработка ошибок при чтении команды
		{
			cin.ignore(numeric_limits<streamsize>::max());
			cin.clear();
		}
		cin.getline(command_buffer, 20);			//Чтение команды
		input_err = cin.fail();
		char prefix = command_buffer[0];
		int command_floor;
		if (prefix == 'F' || prefix == 'C')			//Выделение префикса команды
		{
			input_err = false;
			try
			{
				command_floor = stoi(command_buffer + 1);	//Выделение этажа из команды
			}
			catch (std::invalid_argument&)
			{
				input_err = true;
			}
		}
		else {
			input_err = true;
		}

		if (!input_err)
		{
			emu_command.prefix = prefix;
			emu_command.floor = command_floor;
		}
		else
		{
			cout << "Error in the command. Repeat the input: ";
		}

	} while (input_err);
		
	return emu_command;
}
