#include "euler_cmp.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("For small angles"){
	double eps = 0.001;
	compendulum my_cmp;
	my_cmp.L = 1;
	my_cmp.g = 9.8;
	my_cmp.friction = 0.0;
	my_cmp.m = 1;
	my_cmp.angle0 = 3.14/10; // radians
	my_cmp.velocity = 0.5;
	double **calcs = calculate(my_cmp);
	REQUIRE( fabs(accuracy_period(calcs, my_cmp))<eps);
	my_cmp.friction = 0.5;
	double **calcs = calculate(my_cmp);
	REQUIRE( fabs(accuracy_period(calcs, my_cmp))<eps);
}



TEST_CASE("For big angles"){
	double eps = 0.001;
	compendulum my_cmp;
	my_cmp.L = 1;
	my_cmp.g = 9.8;
	my_cmp.friction = 0.0;
	my_cmp.m = 1;
	my_cmp.angle0 = 1.5; // radians
	my_cmp.velocity = 0.5;
	double **calcs = calculate(my_cmp);
	REQUIRE( fabs(accuracy_period(calcs, my_cmp))>eps);
	my_cmp.friction = 0.5;
	double **calcs = calculate(my_cmp);
	REQUIRE( fabs(accuracy_period(calcs, my_cmp))>eps);
}
