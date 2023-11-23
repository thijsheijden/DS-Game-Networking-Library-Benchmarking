#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

struct Position
{
public:
    float x;
    float y;

    Position()=default;
    Position(float initialX, float initialY)
    {
        x = initialX;
        y = initialY;
    }
};

#endif //CLIENT_COMMON_H
