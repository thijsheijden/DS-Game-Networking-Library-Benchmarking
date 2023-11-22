#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

struct Position
{
public:
    // Constructors
    Position()=default;
    Position(float initialX, float initialY)
    {
        x = initialX;
        y = initialY;
    }

    // Fields
    float x;
    float y;
};

#endif //CLIENT_COMMON_H
