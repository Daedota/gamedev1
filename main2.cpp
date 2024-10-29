#include <stdio.h>
#include <math.h>
#include <windows.h>

#define WIDTH 100   // �ܼ� �ʺ� ����
#define HEIGHT 100  // �ܼ� ���� ����
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// �ܼ� ũ�� ����
void setConsoleSize(int width, int height) {
    HWND console = GetConsoleWindow();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { width, height };
    SetConsoleScreenBufferSize(hConsole, coord);
    MoveWindow(console, 100, 100, width * 8, height * 16, TRUE); // ��ġ�� ũ�� ����
}

class Matrix3x3 {
public:
    float m[3][3];

    Matrix3x3() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = 0;
    }

    void initIdentity() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1 : 0;
    }

    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = 0; // �ʱ�ȭ
                for (int k = 0; k < 3; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
};

class Vector3 {
public:
    float x, y, z;

    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

// ���Ϳ� ��� ���� �Լ�
Vector3 Mul(const Vector3& vec, const Matrix3x3& mat) {
    float newX = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2];
    float newY = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2];
    float newZ = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2]; // w�� ����Ͽ� 3D ���� ǥ��
    return Vector3(newX, newY, newZ);
}

// ��� ���� �Լ�
Matrix3x3 Mul(const Matrix3x3& a, const Matrix3x3& b) {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0; // �ʱ�ȭ
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

Matrix3x3 createTranslationMatrix(float tx, float ty) {
    Matrix3x3 mat;
    mat.initIdentity();
    mat.m[0][2] = tx;
    mat.m[1][2] = ty;
    return mat;
}

Matrix3x3 createRotationMatrix(float angle) {
    Matrix3x3 mat;
    mat.initIdentity();
    float radian = angle * M_PI / 180;
    mat.m[0][0] = cos(radian);
    mat.m[0][1] = -sin(radian);
    mat.m[1][0] = sin(radian);
    mat.m[1][1] = cos(radian);
    return mat;
}

void clearScreen() {
    system("cls");
}

void draw(int angle) {
    clearScreen();

    // P ��ġ
    int pX = WIDTH / 2; // P�� ������ �Ʒ��� �̵�
    int pY = HEIGHT / 2;

    // O�� �ʱ� ��ġ (P���� 5.0f �Ÿ�)
    Vector3 o(0.0f, 5.0f, 0.0f); // O�� �ʱ� ��ġ

    // C�� �Ÿ� (O�� ���� ������ �Ÿ�)
    const float cDistance = 1.5f; // O���� ������ �Ÿ�
    const float oDistance = 5.0f; // P���� O������ �Ÿ�

    // �̵� ��� ���� (P���� �Ÿ�)
    Matrix3x3 translationToP = createTranslationMatrix(pX, pY);

    // O�� ȸ�� ��� ����
    Matrix3x3 rotationO = createRotationMatrix(angle);

    // O�� ��ġ ��ȯ
    Matrix3x3 combinedO = Mul(translationToP, Mul(rotationO, createTranslationMatrix(0, oDistance)));
    o = Mul(o, combinedO);

    // C�� ��ġ ��ȯ (O�� �������� ���� �Ÿ����� ����)
    Matrix3x3 rotationC = createRotationMatrix(angle * 3); // C�� O���� �� ������ ȸ��
    Vector3 c(cDistance, 0, 0); // O�� ������ ���� C�� �ʱ� ��ġ
    c = Mul(c, rotationC); // C�� ��ġ�� ȸ��
    c.x += o.x; // O�� ��ġ�� �߰�
    c.y += o.y;

    // ��� ���� ����
    Matrix3x3 A = createRotationMatrix(angle);
    Matrix3x3 b = createTranslationMatrix(2, 3);
    Matrix3x3 cMatrix = createRotationMatrix(angle / 2); // �̸� ����
    Matrix3x3 d = createTranslationMatrix(1, -1);

    // ���� ����� ���ϱ�
    Matrix3x3 e = Mul(A, Mul(b, Mul(cMatrix, d)));

    // ȭ�� �׸���
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == pX && y == pY) {
                printf("P"); // P ���
            }
            else if ((int)o.x == x && (int)o.y == y) {
                printf("O"); // O ���
            }
            else if ((int)c.x == x && (int)c.y == y) {
                printf("C"); // C ���
            }
            else {
                printf(" "); // �� ���� ���
            }
        }
        printf("\n");
    }
}

int main(void) {
    setConsoleSize(WIDTH, HEIGHT);
    for (int angle = 0; angle < 360; angle += 5) {
        draw(angle); // ȸ�� ������ ���� �׸���
        Sleep(100); // 0.1�� ���
    }
    return 0;
}
