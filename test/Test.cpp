#include <Autogame.hpp>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    autogame::Init("es", 128);
    autogame::Login("Random", "=guill3m=");

    autogame::Update();

    system("pause");
}
