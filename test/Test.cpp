#include <Autogame.hpp>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	autogame::Session(0);
    autogame::Init("es", 128);
    autogame::Login("Random", "=guill3m=");

    autogame::Update();
	autogame::Process();

    system("pause");
}
