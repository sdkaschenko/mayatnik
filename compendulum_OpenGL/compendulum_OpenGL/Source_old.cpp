#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <glut.h>
#include <string> 



#define PI 3.141592653589
#define N 1000
using namespace std;

struct compendulum {
	double L;
	double g;
	double angle0;
	double velocity;
};


double **calculate(compendulum cmp);
double get_maxPhi(double **calc);
double get_maxPPhi(double **calc);
double get_minPhi(double **calc);
double get_minPPhi(double **calc);
double get_maxTime(double **calcs);
double to_rad(double angle);
double round_up(double value, int decimal_places);
void make_phaseGraph(double **calcs, int time, double spacing);
void make_timeGraph(double **calcs, int time, double spacing);
void draw_cmp(double **calcs, compendulum cmp, int time, double spacing);
void outText(float x, float y, string string) {
	//const char *c;
	glRasterPos2f(x, y);
	for (int i = 0; i < string.length();i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
}
void outText24(float x, float y, string string) {
	//const char *c;
	glRasterPos2f(x, y);
	for (int i = 0; i < string.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
void outInfoCmp(double **calcs, int time) {
	double x = -0.9;
	double y = 0.9;
	glColor3d(0.0, 0.0, 0.0);
	outText24(x, y, "Time: " + to_string(calcs[time][0]));
	outText24(x, y - 0.1, "Angle: " + to_string(calcs[time][1]*180/PI ));
	outText24(x, y - 0.2, "Velocity: " + to_string(calcs[time][2] * 180/PI));
}
int main(void)
{

	compendulum my_cmp;
	my_cmp.L = 1;
	my_cmp.g = 9.8;
	my_cmp.angle0 = PI/4 ;
	my_cmp.velocity = 10;
	double **calcs = calculate(my_cmp);
	
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(1080, 720, "Hello World", NULL, NULL);
	glfwSetWindowSizeLimits(window,1080, 720, 1080, 720);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	int time = 0;
	double spacing = 0.9;
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(1.0, 1.0, 1.0, 0.0);

		glClear(GL_COLOR_BUFFER_BIT);
		make_phaseGraph(calcs,time,spacing);
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
	double dt = (double)T / N *2 ;
	//double phi[N] = { 0 };
	//double p_phi[N] = { 0 };
	//double time[N] = { 0 };
	//init 
	double **calculations = new double*[N];
	for (int i = 0; i < N; i++) {
		calculations[i] = new double[3];
	}

	calculations[0][0] = 0; // time column
	calculations[0][1] = cmp.angle0; // angle column
	calculations[0][2] = cmp.velocity; // deriv angle column
	for (int i = 1; i < N; i++) {
		calculations[i][0] = calculations[i - 1][0] + dt;
		calculations[i][2] = calculations[i - 1][2] - cmp.g / cmp.L * sin(calculations[i - 1][1]) * dt; // ???
		calculations[i][1] = calculations[i][2] * dt + calculations[i - 1][1];
	}
	return calculations;
}


void make_phaseGraph(double **calcs, int time, double spacing) {



	double max_phi = get_maxPhi(calcs);
	double max_p_phi = get_maxPPhi(calcs);
	double min_phi = get_minPhi(calcs);
	double min_p_phi = get_minPPhi(calcs);
	double x = 0;
	double y = 0;


	double x_factor = (1 - 0) / (max_phi - min_phi)*spacing;
	double y_factor = (1 - 0) / (max_p_phi - min_p_phi)*spacing;
	double x_transfer = -(max_phi + min_phi) / 2 * x_factor + 0.5;
	double y_transfer = -(max_p_phi + min_p_phi) / 2 * y_factor + 0.5;
	glPointSize(3);
	glBegin(GL_LINES); {
		glColor3d(0.4, 0.4, 0.4);
		for (int i = 0; i < N; i++) {
			//x = calcs[i][1]         / max_phi * x_factor + x_transfer;
			//y = calcs[i][2] / max_p_phi * y_factor + y_transfer;
			
			x = calcs[i][1] * x_factor +x_transfer;
			y = calcs[i][2] *y_factor  +y_transfer;


			/*x = (calcs[i][1]/max_phi*2-1)*x_factor; @todo formula peres4eta!!!!!!!!
			y = (calcs[i][2] /(max_p_phi-min_p_phi) * 2-1)/(max_p_phi-min_p_phi);*/
		//cout << y << endl;
			glVertex2d(x, y);
		}
	}
	//cout << b1 << " " << max_p_phi << " " << min_p_phi << " " << max_p_phi - min_p_phi<< endl;

	glEnd();

	glPointSize(8);
	glBegin(GL_POINTS); {
		glColor3d(1.0, 0.0, 0.0);
		x = calcs[time][1] * x_factor + x_transfer;
		y = calcs[time][2] * y_factor +y_transfer;
		glVertex2d(x, y);
	}
	glEnd();

	//int n_strokes = 9;
	//glBegin(GL_LINES); {
	//	glColor3d(0.0, 0.0, 0.0);
	//	//OX
	//	y = (max_p_phi + min_p_phi) / 2 * y_factor + y_transfer-0.45;
	//	x = min_phi * x_factor + x_transfer;
	//	glVertex2d(min_phi*x_factor  +x_transfer, y);
	//	glVertex2d(max_phi*x_factor +x_transfer, y);

	//	for (int i = 0; i <= n_strokes; i++) {
	//		glVertex2d(x + i * (max_phi-min_phi)*x_factor / n_strokes, y-0.01);
	//		glVertex2d(x + i * (max_phi - min_phi)*x_factor / n_strokes, y +0.01);
	//		glEnd();
	//		outText(x + i * (max_phi - min_phi)*x_factor / n_strokes-0.05, y-0.05, to_string(to_rad( i * (max_phi - min_phi) / n_strokes - min_phi)));
	//		glBegin(GL_LINES);
	//	}
	//	//OY
	//	y = min_p_phi* y_factor + y_transfer;//abs (min_p_phi) ???
	//	x = (max_phi-min_phi )/2* x_factor + x_transfer-0.45;
	//	glVertex2d(x, min_p_phi * y_factor + y_transfer);
	//	glVertex2d(x, max_p_phi * y_factor + y_transfer);
	//	for (int i = 1; i <= n_strokes; i++) {
	//		glVertex2d(x - 0.01, y + i * (max_p_phi-min_p_phi)*y_factor / n_strokes);
	//		glVertex2d(x + 0.01, y + i * (max_p_phi - min_p_phi)* y_factor / n_strokes);
	//		glEnd();
	//		outText(x - 0.1, y + i * (max_p_phi - min_p_phi)* y_factor / n_strokes, to_string(to_rad(i * (max_p_phi - min_p_phi) / n_strokes - min_p_phi)));
	//		glBegin(GL_LINES);
	//	}
	//}glEnd();
	//glColor3d(0.0, 0.0, 0.5);

	//outText(x_transfer + 1 * x_factor - 0.1, y_transfer + 0.05, "angle, rad");
	//outText(x_transfer + 0.05, y_transfer + y_factor, "velocity, rad/s");

}

void make_timeGraph(double **calcs, int time, double spacing) {
	//@todo std::string trimmedString = std::to_string(doubleVal).substr(0, std::to_string(doubleVal).find(".") + precisionVal + 1);
	double x_factor = 1 * spacing;
	double y_factor = 0.5 * spacing;

	double max_phi = get_maxPhi(calcs);
	double max_time = get_maxTime(calcs);
	double x = 0;
	double y = 0;
	double x_transfer = 0;
	double y_transfer = -0.5;



	glPointSize(3);
	glBegin(GL_POINTS); {
		glColor3d(0.4, 0.4, 0.4);
		for (int i = 0; i < N; i++) {
			x = calcs[i][0] / max_time * x_factor;
			y = calcs[i][1] / max_phi * y_factor - 0.5;
			glVertex2d(x, y);
		}
	}
	glEnd();

	glPointSize(8);
	glBegin(GL_POINTS); {
		glColor3d(1.0, 0.0, 0.0);
		x = calcs[time][0] / max_time * x_factor;
		y = calcs[time][1] / max_phi * y_factor - 0.5;
		glVertex2d(x, y);
	}
	glEnd();

	// namings
	int n_strokes = 9;
	glBegin(GL_LINES); {
		glColor3d(0.0, 0.0, 0.0);
		//OX
		glVertex2d(x_transfer, y_transfer);
		glVertex2d(x_transfer + 1 * x_factor, y_transfer);
		for (int i = 1; i <= n_strokes; i++) {
			glVertex2d(x_transfer + i * x_factor / n_strokes, y_transfer-0.01);
			glVertex2d(x_transfer + i * x_factor / n_strokes, y_transfer+0.01);
			glEnd();
			outText(x_transfer + i * x_factor / n_strokes +0.01, y_transfer - 0.04, to_string(round_up(max_time*i /n_strokes,2)));
			glBegin(GL_LINES);
		}
		//OY
		glVertex2d(x_transfer, y_transfer - max_phi * y_factor / 2);
		glVertex2d(x_transfer, y_transfer + max_phi * y_factor / 2);
		for (int i = 0; i <= n_strokes; i++) {
			glVertex2d(x_transfer - 0.01 , y_transfer + i * y_factor / n_strokes);
			glVertex2d(x_transfer + 0.01, y_transfer  + i * y_factor / n_strokes);
			glVertex2d(x_transfer - 0.01, y_transfer - i * y_factor / n_strokes);
			glVertex2d(x_transfer + 0.01, y_transfer - i * y_factor / n_strokes);
			glEnd();
			if (i != 0) {
				outText(x_transfer - 0.1, y_transfer - i * y_factor / n_strokes - 0.01, to_string(round_up(to_rad (-max_phi * i / n_strokes),2)));
			}
			outText(x_transfer - 0.1, y_transfer + i * y_factor / n_strokes - 0.01, to_string(round_up(to_rad(max_phi * i / n_strokes),2)));
			glBegin(GL_LINES);
		}
	}glEnd();
	glColor3d(0.0, 0.0, 0.5);

	outText(x_transfer + 1 * x_factor - 0.1, y_transfer - 0.1, "time, s");
	outText(x_transfer + 0.01, y_transfer + max_phi * y_factor / 2, "angle, rad");



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
	//@MAKE TESTS and dopisat
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

