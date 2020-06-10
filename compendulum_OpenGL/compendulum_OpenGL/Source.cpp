#include <GLFW/glfw3.h> // прекомп. библиотеки OpengL
#include <iostream> 
#include <math.h>
#include <glut.h>//старая библ. для OpenGL
#include <string> 



#define PI 3.141592653589 //число пи
#define N 1000 //число точек для графика. в будущем можно сделать его не константным
using namespace std;

struct compendulum { // маятник
	double L; // длина маятника
	double g; // ускорение свободного падения
	double angle0; // стартовый угол
	double velocity; // стартовая скорость
	double m; // вес маятника
	double friction; // трение
};
double **calculate(compendulum cmp); // функция обсчета методом эйлера
double get_maxPhi(double **calc); 
double get_maxPPhi(double **calc);
double get_minPhi(double **calc);
double get_minPPhi(double **calc);
double get_maxTime(double **calcs);
double to_rad(double angle);
double round_up(double value, int decimal_places);
void make_phaseGraph(double **calcs, int time, double spacing); //функция отрисовки фаз. портрета
void make_timeGraph(double **calcs, int time, double spacing); // функция отрисовки временного графика
void draw_cmp(double **calcs, compendulum cmp, int time, double spacing); // функция отрисовки модели маятника
double accuracy_period(double **calcs, compendulum cmp); //функция для тестирования по периоду... период не изм в случае линейного ДУ, изменяется в случае нелин. ДУ, не измен. в случае нелин ДУ при малых углах
void outText(float x, float y, string string) { //функция отрисовки текста шрифт 10 times new roman
	//const char *c;
	glRasterPos2f(x, y);
	for (int i = 0; i < string.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
}
void outText24(float x, float y, string string) { //функция отрисовки текста шрифт 24 times new roman
	//const char *c;
	glRasterPos2f(x, y);
	for (int i = 0; i < string.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void outInfoCmp(double **calcs, int time) { //функция отрисовки информации о маятнике, текущее время, текущий угол, текущая скорость
	double x = -0.9;
	double y = 0.9;
	glColor3d(0.0, 0.0, 0.0);
	outText24(x, y, "Time: " + to_string(calcs[time][0]));
	outText24(x, y - 0.1, "Angle: " + to_string(calcs[time][1] * 180 / PI));
	outText24(x, y - 0.2, "Velocity: " + to_string(calcs[time][2] * 180 / PI));
}
int main(void)
{
	compendulum my_cmp;
	cout << "Input init params for mathematical compendulum:\n1. Input compendulum length:";
	cin >> my_cmp.L;
	cout << "2. Input compendulum g(for Earth = 9.8): ";
	cin >> my_cmp.g;
	cout << "3. Input compendulum friction( 0 if no friction): ";
	cin >> my_cmp.friction;
	cout << "4. Input compendulum mass: ";
	cin >> my_cmp.m;
	cout << "5. Input compendulum start angle(radians): ";
	cin >> my_cmp.angle0;
	cout << "6. Input compendulum start velocity(radians/s): ";
	cin >> my_cmp.velocity;

	/*my_cmp.L = 1;
	my_cmp.g = 9.8;
	my_cmp.friction = 0.5;
	my_cmp.m = 1;

	my_cmp.angle0 = 1.01*PI;
	my_cmp.velocity = 0.5;*/
	double **calcs = calculate(my_cmp); //calcs - массив для высчитанных точек, время, угол, скорость
	cout << endl << accuracy_period(calcs, my_cmp)<<endl;
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(1080, 720, "Hello World", NULL, NULL); // окно отобржания
	glfwSetWindowSizeLimits(window, 1080, 720, 1080, 720); //фиксированя длина и ширина
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	int time = 0; //счетчки времени
	double spacing = 0.9; // раздел между отображениями
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(1.0, 1.0, 1.0, 0.0); //белый фон

		glClear(GL_COLOR_BUFFER_BIT);
		//основные функции отрисовки
		make_phaseGraph(calcs, time, spacing);
		make_timeGraph(calcs, time, spacing);
		draw_cmp(calcs, my_cmp, time, 0.6);
		outInfoCmp(calcs, time);
		time++;
		time = time % N;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
double  **calculate(compendulum cmp) {
	double T = 2 * PI*sqrt(cmp.L / cmp.g);
	double dt = (double)T / N * 8;
	double **calculations = new double*[N];
	for (int i = 0; i < N; i++) {
		calculations[i] = new double[3];
	}

	calculations[0][0] = 0; // time column
	calculations[0][1] = cmp.angle0; // angle column
	calculations[0][2] = cmp.velocity; // deriv angle column
	for (int i = 1; i < N; i++) {
		calculations[i][0] = calculations[i - 1][0] + dt;
		calculations[i][2] = calculations[i - 1][2] - (cmp.g / cmp.L * sin(calculations[i - 1][1]) + cmp.friction / cmp.m *calculations[i - 1][2])* dt; // основная формула расчета ДУ методом эйлера
		calculations[i][1] = calculations[i][2] * dt + calculations[i - 1][1];
	}
	
	
	return calculations;
}
double accuracy_period(double **calcs, compendulum cmp) {
	//период расчитывается по двум экстремумам функции angle(time)
	double T_analytics = 2 * PI*sqrt(cmp.L / cmp.g);
	double T_calculated = 0;
	double time1_max = 0, time2_max = 0;
	int i = 1;
	while ( (time2_max == 0) ) {
		if (calcs[i - 1][1]<calcs[i][1] && calcs[i][1]>calcs[i + 1][1]) {
			if (time1_max == 0) {
				time1_max = calcs[i][0];
			}
			else {
				time2_max = calcs[i][0];
			}
		}
		i++;
	}
	return T_analytics - (time2_max - time1_max);
}

void make_phaseGraph(double **calcs, int time, double spacing) {



	double max_phi = get_maxPhi(calcs);
	double max_p_phi = get_maxPPhi(calcs);
	double min_phi = get_minPhi(calcs);
	double min_p_phi = get_minPPhi(calcs);
	double x = 0;
	double y = 0;

	//нормировка и относительная система координат (формулы из ан. геом.)
	double x_factor = (1 - 0) / (max_phi - min_phi)*spacing;
	double y_factor = (1 - 0) / (max_p_phi - min_p_phi)*spacing;
	double x_transfer = -(max_phi + min_phi) / 2 * x_factor + 0.5;
	double y_transfer = -(max_p_phi + min_p_phi) / 2 * y_factor + 0.5; 
	glLineWidth(3);
	glBegin(GL_LINE_STRIP); {
		glColor3d(0.4, 0.4, 0.4);
		for (int i = 0; i < N; i++) {
			x = calcs[i][1] * x_factor + x_transfer;
			y = calcs[i][2] * y_factor + y_transfer;
			glVertex2d(x, y);
		}
	}
	glEnd();

	glPointSize(8);
	glBegin(GL_POINTS); {
		glColor3d(1.0, 0.0, 0.0);
		x = calcs[time][1] * x_factor + x_transfer;
		y = calcs[time][2] * y_factor + y_transfer;
		glVertex2d(x, y);
	}
	glEnd();
	outText(x - 0.2, y - 0.1, "angle: " + to_string((int)to_rad(calcs[time][1]) % 360));
	outText(x - 0.2, y - 0.05, "velocity: " + to_string((int)to_rad(calcs[time][2]) % 360));
}

void make_timeGraph(double **calcs, int time, double spacing) {



	double max_phi = get_maxPhi(calcs);
	double min_phi = get_minPhi(calcs);
	double max_time = get_maxTime(calcs);
	double x = 0;
	double y = 0;


	double x_factor = (1 - 0) / (max_time - 0)*spacing;
	double y_factor = (1 - 0) / (max_phi - min_phi)*spacing;
	double x_transfer = -(max_time + 0) / 2 * x_factor + 0.5;
	double y_transfer = -(max_phi + min_phi) / 2 * y_factor - 0.5;


	glLineWidth(3);
	glBegin(GL_LINE_STRIP); {
		glColor3d(0.4, 0.4, 0.4);
		for (int i = 0; i < N; i++) {
			x = calcs[i][0] * x_factor + x_transfer;
			y = calcs[i][1] * y_factor + y_transfer;
			glVertex2d(x, y);
		}
	}
	glEnd();

	glPointSize(8);
	glBegin(GL_POINTS); {
		glColor3d(1.0, 0.0, 0.0);
		x = calcs[time][0] * x_factor + x_transfer;
		y = calcs[time][1] * y_factor + y_transfer;
		glVertex2d(x, y);
	}
	glEnd();
	outText(x - 0.15, y + 0.05, "time: " + to_string(calcs[time][0]));
	outText(x - 0.15, y, "angle: " + to_string((int)to_rad(calcs[time][1]) % 360));
}
double round_up(double value, int decimal_places) {
	const double multiplier = std::pow(10.0, decimal_places);
	return std::ceil(value * multiplier) / multiplier;
}
double get_maxTime(double **calcs) {
	return calcs[N - 1][0];
}
double get_maxPhi(double **calc) {
	double max = calc[0][1];

	for (int i = 0; i < N; i++) {
		if (calc[i][1] > max) {
			max = calc[i][1];
		}
	}

	return max;
}
double get_maxPPhi(double **calc) {
	double max = calc[0][2];

	for (int i = 0; i < N; i++) {
		if (calc[i][2] > max) {
			max = calc[i][2];
		}
	}

	return max;
}
double get_minPhi(double **calc) {
	double min = calc[0][1];

	for (int i = 0; i < N; i++) {
		if (calc[i][1] < min) {
			min = calc[i][1];
		}
	}

	return min;
}
double get_minPPhi(double **calc) {
	double min = calc[0][2];

	for (int i = 0; i < N; i++) {
		if (calc[i][2] < min) {
			min = calc[i][2];
		}
	}

	return min;
}
double to_rad(double angle) {
	return angle * 180 / PI;
}
void draw_cmp(double **calcs, compendulum cmp, int time, double spacing) {
	double y = 0, x = 0;
	double x_factor = 0.5 * spacing;
	double y_factor = 0.5 * spacing * 1080 / 720;
	double x_transfer = -0.5;
	double y_transfer = 0.0;

	glBegin(GL_LINES); {
		glColor3d(0.0, 0.0, 0.0);
		x = cmp.L * sin(calcs[time][1]) / cmp.L * x_factor + x_transfer;
		y = -cmp.L * cos(calcs[time][1]) / cmp.L * y_factor + y_transfer;
		//cout << sqrt( pow(x-x_transfer,2) + pow(y-y_transfer,2))<<endl;
		glVertex2d(x_transfer, y_transfer);
		glVertex2d(x, y);
	}glEnd();

	glPointSize(25);
	glBegin(GL_POINTS); {
		glColor3d(1.0, 0.0, 0.0);

		x = cmp.L * sin(calcs[time][1]) / cmp.L * x_factor + x_transfer;
		y = -cmp.L * cos(calcs[time][1]) / cmp.L * y_factor + y_transfer;
		glVertex2d(x, y);
	}
	glEnd();
}

