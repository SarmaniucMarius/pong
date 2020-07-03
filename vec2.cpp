struct vec2 {
    float x;
    float y;
};

vec2 operator*(vec2 a, float b)
{
    vec2 result;
    result.x = a.x * b;
    result.y = a.y * b;
    return result;
}

vec2 &operator*=(vec2 &a, float b)
{
    a = a * b;
    return a;
}

vec2 operator+(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

vec2 &operator+=(vec2 &a, vec2 b)
{
    a = a + b;
    return a;
}

vec2 operator+(vec2 a, int b)
{
    vec2 result;
    result.x = a.x + b;
    result.y = a.y + b;
    return result;
}

vec2 operator-(vec2 a, vec2 b)
{
    vec2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

vec2 operator-(vec2 a, int b)
{
    vec2 result;
    result.x = a.x - b;
    result.y = a.y - b;
    return result;
}

vec2 operator-(vec2 a)
{
    vec2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}