//#include <vld.h>
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <iostream>
#include "World.h"
#include "Clock.h"

int main(){
	World world;
	Clock clock;
	clock.restart();

	while (world.IsRunning()){
		world.Update(clock.restart());
	}
	return 0;
}