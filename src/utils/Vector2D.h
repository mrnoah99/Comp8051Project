class Vector2D {
    public:
        float x = 0.0f, y = 0.0f;
        
        Vector2D() {
            x = 0.0f;
            y = 0.0f;
        }

        Vector2D(float x, float y): x(x), y(y) {}

        Vector2D operator+(const Vector2D& vec) const;

        Vector2D operator-(const Vector2D& vec) const;

        //member operator
        //important that Vector2D is on the left and scalar is on the right
        Vector2D operator*(float scalar) const;

        //vector2D on the right and float on left
        //make as non member function using friend
        friend Vector2D operator*(float scalar, Vector2D vec);

        Vector2D operator/(float scalar) const;

        friend Vector2D operator/(float scalar, Vector2D vec);

        Vector2D& operator*=(float scalar);

        Vector2D& operator/=(float scalar);

        Vector2D& operator+=(const Vector2D& vec);

        Vector2D& operator-=(const Vector2D& vec);

        bool operator==(Vector2D vec);

        bool operator!=(Vector2D vec);

        friend Vector2D operator-(Vector2D vec);

        Vector2D& normalize();

        float dot(Vector2D vec);
};