#include <stdio.h>
#include <math.h>
#include <windows.h>

#define WIDTH 100   // 콘솔 너비 증가
#define HEIGHT 100  // 콘솔 높이 증가
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 콘솔 크기 설정
void setConsoleSize(int width, int height) {
    HWND console = GetConsoleWindow();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { width, height };
    SetConsoleScreenBufferSize(hConsole, coord);
    MoveWindow(console, 100, 100, width * 8, height * 16, TRUE); // 위치와 크기 조정
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
                result.m[i][j] = 0; // 초기화
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

// 벡터와 행렬 곱셈 함수
Vector3 Mul(const Vector3& vec, const Matrix3x3& mat) {
    float newX = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2];
    float newY = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2];
    float newZ = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2]; // w를 사용하여 3D 벡터 표현
    return Vector3(newX, newY, newZ);
}

// 행렬 곱셈 함수
Matrix3x3 Mul(const Matrix3x3& a, const Matrix3x3& b) {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0; // 초기화
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

    // P 위치
    int pX = WIDTH / 2; // P를 오른쪽 아래로 이동
    int pY = HEIGHT / 2;

    // O의 초기 위치 (P에서 5.0f 거리)
    Vector3 o(0.0f, 5.0f, 0.0f); // O의 초기 위치

    // C의 거리 (O에 대한 고정된 거리)
    const float cDistance = 1.5f; // O에서 일정한 거리
    const float oDistance = 5.0f; // P에서 O까지의 거리

    // 이동 행렬 생성 (P와의 거리)
    Matrix3x3 translationToP = createTranslationMatrix(pX, pY);

    // O의 회전 행렬 생성
    Matrix3x3 rotationO = createRotationMatrix(angle);

    // O의 위치 변환
    Matrix3x3 combinedO = Mul(translationToP, Mul(rotationO, createTranslationMatrix(0, oDistance)));
    o = Mul(o, combinedO);

    // C의 위치 변환 (O를 기준으로 일정 거리에서 공전)
    Matrix3x3 rotationC = createRotationMatrix(angle * 3); // C는 O보다 더 빠르게 회전
    Vector3 c(cDistance, 0, 0); // O의 주위를 도는 C의 초기 위치
    c = Mul(c, rotationC); // C의 위치를 회전
    c.x += o.x; // O의 위치에 추가
    c.y += o.y;

    // 행렬 정의 예시
    Matrix3x3 A = createRotationMatrix(angle);
    Matrix3x3 b = createTranslationMatrix(2, 3);
    Matrix3x3 cMatrix = createRotationMatrix(angle / 2); // 이름 변경
    Matrix3x3 d = createTranslationMatrix(1, -1);

    // 여러 행렬을 곱하기
    Matrix3x3 e = Mul(A, Mul(b, Mul(cMatrix, d)));

    // 화면 그리기
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == pX && y == pY) {
                printf("P"); // P 출력
            }
            else if ((int)o.x == x && (int)o.y == y) {
                printf("O"); // O 출력
            }
            else if ((int)c.x == x && (int)c.y == y) {
                printf("C"); // C 출력
            }
            else {
                printf(" "); // 빈 공간 출력
            }
        }
        printf("\n");
    }
}

int main(void) {
    setConsoleSize(WIDTH, HEIGHT);
    for (int angle = 0; angle < 360; angle += 5) {
        draw(angle); // 회전 각도에 따라 그리기
        Sleep(100); // 0.1초 대기
    }
    return 0;
}
