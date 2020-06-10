#include "euler_cmp.hpp"
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
		calculations[i][2] = calculations[i - 1][2] - (cmp.g / cmp.L * sin(calculations[i - 1][1]) + cmp.friction / cmp.m *calculations[i - 1][2])* dt; // ???
		calculations[i][1] = calculations[i][2] * dt + calculations[i - 1][1];
	}


	return calculations;
}
double accuracy_period(double **calcs, compendulum cmp) {
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
